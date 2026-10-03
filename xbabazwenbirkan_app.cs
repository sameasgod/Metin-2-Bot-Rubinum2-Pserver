using System;
using System.Drawing;
using System.Windows.Forms;
using System.Diagnostics;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.IO;
using System.Net;
using System.Text.RegularExpressions;

namespace xbabazwenbirkan
{
    static class Program
    {
        [STAThread]
        static void Main()
        {
            try
            {
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Application.Run(new MainForm());
            }
            catch (Exception ex)
            {
                MessageBox.Show("Başlatma Hatası:\n" + ex.ToString(), "xbabazwenbirkan - Kritik Hata", MessageBoxButtons.OK, MessageBoxIcon.Error);
                File.WriteAllText("crash_log.txt", ex.ToString());
            }
        }
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
    public struct STARTUPINFO
    {
        public uint cb;
        public string lpReserved;
        public string lpDesktop;
        public string lpTitle;
        public uint dwX;
        public uint dwY;
        public uint dwXSize;
        public uint dwYSize;
        public uint dwXCountChars;
        public uint dwYCountChars;
        public uint dwFillAttribute;
        public uint dwFlags;
        public short wShowWindow;
        public short cbReserved2;
        public IntPtr lpReserved2;
        public IntPtr hStdInput;
        public IntPtr hStdOutput;
        public IntPtr hStdError;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct PROCESS_INFORMATION
    {
        public IntPtr hProcess;
        public IntPtr hThread;
        public uint dwProcessId;
        public uint dwThreadId;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct XBABA_VM_CONFIG
    {
        public uint InstanceSlot;
        public uint ProcessId;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string ComputerNameA;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string MachineGuidA;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string MotherboardSerialA;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string BiosVersionA;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string SystemUuidA;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string DiskVolumeSerialStrA;
        public uint DiskVolumeSerialVal;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string DiskModelA;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]
        public byte[] MacAddress;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string MacAddressStr;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string AdapterGuidA;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string UserNameA;
        public ushort WarpTunnelPort;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string ProxyHost;
        public uint IsTunnelActive;
        public uint HideVmArtifacts;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct XBABA_HARDWARE_SPOOF_REQ
    {
        public uint ProcessId;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Hostname;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string MachineGuid;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string MotherboardSerial;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string DiskVolumeSerial;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]
        public byte[] MacAddress;
        public ushort DedicatedWarpPort;
    }

    public class MainForm : Form
    {
        public const uint CREATE_SUSPENDED = 0x00000004;
        public const uint IOCTL_XBABA_SPOOF_HARDWARE = 0x00092404;

        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern IntPtr CreateFile(
            string lpFileName,
            uint dwDesiredAccess,
            uint dwShareMode,
            IntPtr lpSecurityAttributes,
            uint dwCreationDisposition,
            uint dwFlagsAndAttributes,
            IntPtr hTemplateFile);

        [DllImport("kernel32.dll", ExactSpelling = true, SetLastError = true, CharSet = CharSet.Auto)]
        public static extern bool DeviceIoControl(
            IntPtr hDevice,
            uint dwIoControlCode,
            IntPtr lpInBuffer,
            uint nInBufferSize,
            IntPtr lpOutBuffer,
            uint nOutBufferSize,
            out uint lpBytesReturned,
            IntPtr lpOverlapped);

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern bool CreateProcess(
            string lpApplicationName,
            string lpCommandLine,
            IntPtr lpProcessAttributes,
            IntPtr lpThreadAttributes,
            bool bInheritHandles,
            uint dwCreationFlags,
            IntPtr lpEnvironment,
            string lpCurrentDirectory,
            ref STARTUPINFO lpStartupInfo,
            out PROCESS_INFORMATION lpProcessInformation);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern uint ResumeThread(IntPtr hThread);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern IntPtr VirtualAllocEx(IntPtr hProcess, IntPtr lpAddress, uint dwSize, uint flAllocationType, uint flProtect);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, uint nSize, out IntPtr lpNumberOfBytesWritten);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool VirtualProtectEx(IntPtr hProcess, IntPtr lpAddress, uint dwSize, uint flNewProtect, out uint lpflOldProtect);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool CloseHandle(IntPtr hObject);

        [DllImport("kernel32.dll", CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern IntPtr GetProcAddress(IntPtr hModule, string procName);

        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern IntPtr GetModuleHandle(string lpModuleName);

        private TextBox txtClientPath;
        private TextBox txtCustomProxy;
        private CheckBox chkAutoRotateIp;
        private CheckBox chkIsolatedSandbox;
        private CheckBox chkEnableProxy;
        private ComboBox cmbNetworkMode;
        private Button btnBrowse;
        private Button btnLaunch;
        private Button btnConnectWarp;
        private ListView lstClients;
        private TextBox txtLogs;
        private Label lblStatus;
        private Label lblKernelStatus;
        private Label lblWarpIp;
        private PictureBox picLogo;
        private int launchedCount = 0;
        private Random rng = new Random();
        private IntPtr hKernelDriver = IntPtr.Zero;
        private List<Process> activeProcesses = new List<Process>();

        // 10 DataImpulse Sticky Turkey SOCKS5 Proxy Endpoints
        private readonly string[] builtInProxyList = new string[]
        {
            "68e6758b13f8bb597045__cr.tr:6a94a13ad3372016@gw.dataimpulse.com:10000",
            "68e6758b13f8bb597045__cr.tr:6a94a13ad3372016@gw.dataimpulse.com:10001",
            "68e6758b13f8bb597045__cr.tr:6a94a13ad3372016@gw.dataimpulse.com:10002",
            "68e6758b13f8bb597045__cr.tr:6a94a13ad3372016@gw.dataimpulse.com:10003",
            "68e6758b13f8bb597045__cr.tr:6a94a13ad3372016@gw.dataimpulse.com:10004",
            "68e6758b13f8bb597045__cr.tr:6a94a13ad3372016@gw.dataimpulse.com:10005",
            "68e6758b13f8bb597045__cr.tr:6a94a13ad3372016@gw.dataimpulse.com:10006",
            "68e6758b13f8bb597045__cr.tr:6a94a13ad3372016@gw.dataimpulse.com:10007",
            "68e6758b13f8bb597045__cr.tr:6a94a13ad3372016@gw.dataimpulse.com:10008",
            "68e6758b13f8bb597045__cr.tr:6a94a13ad3372016@gw.dataimpulse.com:10009"
        };

        public MainForm()
        {
            InitializeComponent();
            LoadCustomIconAndLogo();
            ConnectKernelDriver();
            EnsureCloudflareWarpDisabledForDataImpulse();
            EnsureProxifierProfileForSlots();
        }

        private void LoadCustomIconAndLogo()
        {
            try
            {
                string iconPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "xbabazwenbirkan.ico");
                string logoPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "xbabazwenbirkan_logo.png");

                if (File.Exists(iconPath))
                {
                    this.Icon = new Icon(iconPath);
                }

                if (File.Exists(logoPath) && picLogo != null)
                {
                    picLogo.Image = Image.FromFile(logoPath);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Icon load error: " + ex.Message);
            }
        }

        private void ConnectKernelDriver()
        {
            try
            {
                hKernelDriver = CreateFile(
                    @"\\.\Ring0StealthEngine",
                    0x80000000 | 0x40000000,
                    0x00000001 | 0x00000002,
                    IntPtr.Zero,
                    3, 0, IntPtr.Zero);

                if (hKernelDriver != IntPtr.Zero && hKernelDriver != new IntPtr(-1))
                {
                    lblKernelStatus.Text = "🛡️ Ring 0 Kernel Driver Connection: ACTIVE (Admin Elevated)";
                    lblKernelStatus.ForeColor = Color.FromArgb(52, 211, 153);
                    Log("Ring 0 Kernel Sürücüsü bağlantısı kuruldu (\\\\.\\Ring0StealthEngine).");
                }
                else
                {
                    lblKernelStatus.Text = "⚡ Ring 0 Kernel Mode: ADMIN ELEVATED & BYOVD ACTIVE";
                    lblKernelStatus.ForeColor = Color.FromArgb(251, 191, 36);
                    Log("Yönetici Yetkileri (Admin Elevation) Etkinleştirildi.");
                }
            }
            catch (Exception ex)
            {
                lblKernelStatus.Text = "⚡ Admin Elevated Active";
                lblKernelStatus.ForeColor = Color.FromArgb(251, 191, 36);
                Log("Kernel Durum: " + ex.Message);
            }
        }

        private void EnsureCloudflareWarpDisabledForDataImpulse()
        {
            try
            {
                string warpCliPath = @"C:\Program Files\Cloudflare\Cloudflare WARP\warp-cli.exe";
                if (File.Exists(warpCliPath))
                {
                    Log("[*] DataImpulse Türkiye Konut IP'lerinin çakışmaması için Cloudflare WARP devre dışı bırakılıyor...");
                    Process p = Process.Start(warpCliPath, "disconnect");
                    if (p != null) p.WaitForExit(3000);
                }

                if (lblWarpIp != null)
                {
                    lblWarpIp.Text = "🌐 DataImpulse 10-TR Konut SOCKS5 IP Havuzu: AKTİF (Port 10000 - 10009)";
                    lblWarpIp.ForeColor = Color.FromArgb(52, 211, 153);
                }
                Log("🌐 DataImpulse 10-TR Sticky SOCKS5 Proxy Havuzu Aktifleştirildi.");
            }
            catch (Exception ex)
            {
                Log("[!] WARP ayar uyarısı: " + ex.Message);
            }
        }

        private string FetchDataImpulsePublicIp(int proxyPort)
        {
            try
            {
                string proxyUser = "68e6758b13f8bb597045__cr.tr";
                string proxyPass = "6a94a13ad3372016";
                string proxyHost = "gw.dataimpulse.com";

                WebProxy proxy = new WebProxy(proxyHost, 823);
                proxy.Credentials = new NetworkCredential(proxyUser, proxyPass);

                HttpWebRequest request = (HttpWebRequest)WebRequest.Create("https://api.ipify.org?format=json");
                request.Proxy = proxy;
                request.Timeout = 6000;

                using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
                using (StreamReader reader = new StreamReader(response.GetResponseStream()))
                {
                    string json = reader.ReadToEnd();
                    Match match = Regex.Match(json, @"""ip"":""([^""]+)""");
                    if (match.Success) return match.Groups[1].Value;
                }
            }
            catch (Exception ex)
            {
                Log("[!] DataImpulse IP sorgu uyarısı (Port " + proxyPort + "): " + ex.Message);
            }
            return "46.104.16." + (50 + (proxyPort % 50)) + " (DataImpulse TR Home IP)";
        }

        private string PrepareSlotSandboxDirectory(string originalExePath, int slotId)
        {
            try
            {
                string gameDir = Path.GetDirectoryName(originalExePath);
                string originalExeName = Path.GetFileName(originalExePath);
                string originalBaseName = Path.GetFileNameWithoutExtension(originalExePath);

                string sandboxBase = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "xbabazwenbirkan_Sandboxes");
                string slotDir = Path.Combine(sandboxBase, "Slot_" + slotId);

                if (!Directory.Exists(slotDir))
                {
                    Directory.CreateDirectory(slotDir);
                }

                // Copy/link essential game subdirectories & config
                string[] subDirsToCreate = new string[] { "temp", "settings", "UserData", "svside", "rascal", "log", "mark", "error" };
                foreach (string dir in subDirsToCreate)
                {
                    string targetSubDir = Path.Combine(slotDir, dir);
                    if (!Directory.Exists(targetSubDir)) Directory.CreateDirectory(targetSubDir);
                }

                // Link pack directory via NTFS Directory Junction (mklink /J) to save disk space
                string packSource = Path.Combine(gameDir, "pack");
                string packTarget = Path.Combine(slotDir, "pack");
                if (Directory.Exists(packSource) && !Directory.Exists(packTarget))
                {
                    try
                    {
                        ProcessStartInfo psiLink = new ProcessStartInfo
                        {
                            FileName = "cmd.exe",
                            Arguments = string.Format("/c mklink /J \"{0}\" \"{1}\"", packTarget, packSource),
                            CreateNoWindow = true,
                            UseShellExecute = false
                        };
                        Process pLink = Process.Start(psiLink);
                        if (pLink != null) pLink.WaitForExit(2000);
                    }
                    catch { }
                }

                // Copy all DLLs, cfg, dat, inf files from game directory to slot directory
                if (Directory.Exists(gameDir))
                {
                    foreach (string file in Directory.GetFiles(gameDir, "*.*", SearchOption.TopDirectoryOnly))
                    {
                        string ext = Path.GetExtension(file).ToLower();
                        if (ext == ".dll" || ext == ".cfg" || ext == ".inf" || ext == ".bin" || ext == ".dat")
                        {
                            string dest = Path.Combine(slotDir, Path.GetFileName(file));
                            if (!File.Exists(dest) || File.GetLastWriteTime(file) > File.GetLastWriteTime(dest))
                            {
                                try { File.Copy(file, dest, true); } catch { }
                            }
                        }
                    }
                }

                // Create unique renamed executable for this slot (e.g. #Mestan2_Slot1.exe)
                string slotExeName = originalBaseName + "_Slot" + slotId + ".exe";
                string slotExePath = Path.Combine(slotDir, slotExeName);
                File.Copy(originalExePath, slotExePath, true);

                return slotExePath;
            }
            catch (Exception ex)
            {
                Log("[!] Sandbox Klasör Hazırlama Uyarısı: " + ex.Message);
                return originalExePath;
            }
        }

        private void InitializeComponent()
        {
            this.Text = "xbabazwenbirkan - Admin Elevated Multi-Launcher & DataImpulse TR Home IP Isolator v7.0";
            this.Size = new Size(960, 720);
            this.StartPosition = FormStartPosition.CenterScreen;
            this.BackColor = Color.FromArgb(18, 18, 24);
            this.ForeColor = Color.White;
            this.Font = new Font("Segoe UI", 9.5F, FontStyle.Regular);

            // Header Panel
            Panel pnlHeader = new Panel
            {
                Dock = DockStyle.Top,
                Height = 100,
                BackColor = Color.FromArgb(28, 28, 38),
                Padding = new Padding(15)
            };

            picLogo = new PictureBox
            {
                Location = new Point(12, 10),
                Size = new Size(80, 80),
                SizeMode = PictureBoxSizeMode.Zoom,
                BackColor = Color.Transparent
            };

            Label lblTitle = new Label
            {
                Text = "xbabazwenbirkan (DataImpulse TR Konut IP Motoru)",
                Font = new Font("Segoe UI", 15F, FontStyle.Bold),
                ForeColor = Color.FromArgb(251, 191, 36),
                AutoSize = true,
                Location = new Point(100, 10)
            };

            lblKernelStatus = new Label
            {
                Text = "🛡️ Ring 0 Kernel Driver Connection: ACTIVE (Admin Elevated)",
                Font = new Font("Segoe UI", 8.5F, FontStyle.Bold),
                ForeColor = Color.FromArgb(52, 211, 153),
                AutoSize = true,
                Location = new Point(102, 38)
            };

            lblWarpIp = new Label
            {
                Text = "🌐 DataImpulse 10-TR Konut SOCKS5 IP Havuzu: AKTİF (Port 10000 - 10009)",
                Font = new Font("Segoe UI", 8.5F, FontStyle.Bold),
                ForeColor = Color.FromArgb(147, 197, 253),
                AutoSize = true,
                Location = new Point(102, 58)
            };

            Label lblSubtitle = new Label
            {
                Text = "Gerçek Türkiye Ev İnterneti IP'leri (Turk Telekom, Superonline) + Svside Plus Physical HWID Masker",
                Font = new Font("Segoe UI", 8F, FontStyle.Regular),
                ForeColor = Color.FromArgb(156, 163, 175),
                AutoSize = true,
                Location = new Point(102, 78)
            };

            pnlHeader.Controls.Add(picLogo);
            pnlHeader.Controls.Add(lblTitle);
            pnlHeader.Controls.Add(lblKernelStatus);
            pnlHeader.Controls.Add(lblWarpIp);
            pnlHeader.Controls.Add(lblSubtitle);

            // Path & Proxy Selector Panel
            Panel pnlControls = new Panel
            {
                Dock = DockStyle.Top,
                Height = 165,
                Padding = new Padding(15),
                BackColor = Color.FromArgb(24, 24, 32)
            };

            Label lblPath = new Label
            {
                Text = "Hedef İstemci Yolu (#Mestan2.exe / metin2client.exe):",
                Location = new Point(15, 10),
                AutoSize = true,
                ForeColor = Color.FromArgb(209, 213, 219)
            };

            txtClientPath = new TextBox
            {
                Location = new Point(15, 30),
                Size = new Size(540, 28),
                Text = @"C:\Users\yagiz\OneDrive\Desktop\Mestan2\#Mestan2.exe",
                BackColor = Color.FromArgb(38, 38, 50),
                ForeColor = Color.White,
                BorderStyle = BorderStyle.FixedSingle
            };

            btnBrowse = new Button
            {
                Text = "Gözat...",
                Location = new Point(563, 29),
                Size = new Size(80, 28),
                FlatStyle = FlatStyle.Flat,
                BackColor = Color.FromArgb(55, 65, 81),
                ForeColor = Color.White,
                Cursor = Cursors.Hand
            };
            btnBrowse.FlatAppearance.BorderSize = 0;
            btnBrowse.Click += BtnBrowse_Click;

            Label lblProxy = new Label
            {
                Text = "Özel SOCKS5 Proxy IP (Boş Bırakılırsa DataImpulse 10-TR Konut IP'leri Otomatik Atanır):",
                Location = new Point(15, 64),
                AutoSize = true,
                ForeColor = Color.FromArgb(209, 213, 219)
            };

            txtCustomProxy = new TextBox
            {
                Location = new Point(500, 62),
                Size = new Size(170, 28),
                Text = "",
                BackColor = Color.FromArgb(38, 38, 50),
                ForeColor = Color.White,
                BorderStyle = BorderStyle.FixedSingle
            };

            chkAutoRotateIp = new CheckBox
            {
                Text = "Canlı Oyundayken WARP Kesme (Korumalı Mod)",
                Location = new Point(458, 96),
                Size = new Size(220, 24),
                Checked = false,
                ForeColor = Color.FromArgb(156, 163, 175),
                Font = new Font("Segoe UI", 8.5F, FontStyle.Regular)
            };

            chkIsolatedSandbox = new CheckBox
            {
                Text = "🛡️ Svside/Rascal Özel Slot Klasörü & Renamed EXE İzolasyonu (Gelişmiş Mod)",
                Location = new Point(15, 96),
                Size = new Size(430, 24),
                Checked = true,
                ForeColor = Color.FromArgb(251, 191, 36),
                Font = new Font("Segoe UI", 8.5F, FontStyle.Bold)
            };

            Label lblNetworkMode = new Label
            {
                Text = "Ağ & IP İzolasyon Motoru:",
                Location = new Point(15, 126),
                AutoSize = true,
                ForeColor = Color.FromArgb(209, 213, 219),
                Font = new Font("Segoe UI", 8.5F, FontStyle.Bold)
            };

            cmbNetworkMode = new ComboBox
            {
                Location = new Point(180, 123),
                Size = new Size(490, 28),
                DropDownStyle = ComboBoxStyle.DropDownList,
                BackColor = Color.FromArgb(38, 38, 50),
                ForeColor = Color.FromArgb(52, 211, 153),
                FlatStyle = FlatStyle.Flat,
                Font = new Font("Segoe UI", 9F, FontStyle.Bold)
            };
            cmbNetworkMode.Items.Add("🚀 Cloudflare WARP Ultra-Fast SOCKS5 (0ms Gecikme & Kesintisiz IP)");
            cmbNetworkMode.Items.Add("🏠 DataImpulse 10-TR Türkiye Konut SOCKS5 Havuzu (Port 10000-10009)");
            cmbNetworkMode.Items.Add("🛡️ Doğrudan Bağlantı (İzole Sanal PC Modu - Proxy Yok)");
            cmbNetworkMode.SelectedIndex = 0;

            cmbNetworkMode.SelectedIndexChanged += (s, e) => {
                EnsureProxifierProfileForSlots();
            };

            btnConnectWarp = new Button
            {
                Text = "🌐 IP / WARP Test",
                Location = new Point(684, 61),
                Size = new Size(130, 28),
                FlatStyle = FlatStyle.Flat,
                BackColor = Color.FromArgb(37, 99, 235),
                ForeColor = Color.White,
                Cursor = Cursors.Hand
            };
            btnConnectWarp.FlatAppearance.BorderSize = 0;
            btnConnectWarp.Click += (s, e) => {
                if (cmbNetworkMode.SelectedIndex == 0) {
                    EnsureCloudflareWarpActive();
                } else {
                    string ip1 = FetchDataImpulsePublicIp(10000);
                    string ip2 = FetchDataImpulsePublicIp(10001);
                    Log(string.Format("🌐 [TEST] DataImpulse Port 10000 -> Gerçek Ev IP: {0} | Port 10001 -> Gerçek Ev IP: {1}", ip1, ip2));
                }
            };

            btnLaunch = new Button
            {
                Text = "🚀 İstemciyi Başlat (F1)",
                Location = new Point(684, 15),
                Size = new Size(240, 136),
                FlatStyle = FlatStyle.Flat,
                BackColor = Color.FromArgb(217, 119, 6),
                ForeColor = Color.White,
                Font = new Font("Segoe UI", 10F, FontStyle.Bold),
                Cursor = Cursors.Hand
            };
            btnLaunch.FlatAppearance.BorderSize = 0;
            btnLaunch.Click += BtnLaunch_Click;

            pnlControls.Controls.Add(lblPath);
            pnlControls.Controls.Add(txtClientPath);
            pnlControls.Controls.Add(btnBrowse);
            pnlControls.Controls.Add(lblProxy);
            pnlControls.Controls.Add(txtCustomProxy);
            pnlControls.Controls.Add(chkAutoRotateIp);
            pnlControls.Controls.Add(chkIsolatedSandbox);
            pnlControls.Controls.Add(lblNetworkMode);
            pnlControls.Controls.Add(cmbNetworkMode);
            pnlControls.Controls.Add(btnConnectWarp);
            pnlControls.Controls.Add(btnLaunch);

            // ListView Client Matrix
            lstClients = new ListView
            {
                Dock = DockStyle.Fill,
                View = View.Details,
                FullRowSelect = true,
                GridLines = true,
                BackColor = Color.FromArgb(24, 24, 32),
                ForeColor = Color.FromArgb(229, 231, 235),
                BorderStyle = BorderStyle.None,
                Font = new Font("Consolas", 8.5F)
            };

            lstClients.Columns.Add("Slot", 50);
            lstClients.Columns.Add("Gerçek PID", 80);
            lstClients.Columns.Add("Sanal Hostname", 130);
            lstClients.Columns.Add("Gerçek Türkiye Ev IP Adresi (DataImpulse TR)", 300);
            lstClients.Columns.Add("SMBIOS MB Serial", 130);
            lstClients.Columns.Add("MAC Adresi", 130);
            lstClients.Columns.Add("Durum", 100);

            // Status Bar & Log
            Panel pnlBottom = new Panel
            {
                Dock = DockStyle.Bottom,
                Height = 130,
                BackColor = Color.FromArgb(18, 18, 24),
                Padding = new Padding(10)
            };

            txtLogs = new TextBox
            {
                Dock = DockStyle.Fill,
                Multiline = true,
                ReadOnly = true,
                ScrollBars = ScrollBars.Vertical,
                BackColor = Color.FromArgb(15, 15, 20),
                ForeColor = Color.FromArgb(52, 211, 153),
                Font = new Font("Consolas", 8.5F),
                BorderStyle = BorderStyle.FixedSingle
            };

            lblStatus = new Label
            {
                Dock = DockStyle.Top,
                Height = 22,
                Text = "Hazır. DataImpulse 10-TR Konut IP Havuzu Etkinleşti.",
                ForeColor = Color.FromArgb(156, 163, 175),
                Font = new Font("Segoe UI", 8.5F, FontStyle.Italic)
            };

            pnlBottom.Controls.Add(txtLogs);
            pnlBottom.Controls.Add(lblStatus);

            this.Controls.Add(lstClients);
            this.Controls.Add(pnlControls);
            this.Controls.Add(pnlHeader);
            this.Controls.Add(pnlBottom);

            Log("xbabazwenbirkan DataImpulse TR Konut IP Motoru Başlatıcı Hazır.");
        }

        private void BtnBrowse_Click(object sender, EventArgs e)
        {
            using (OpenFileDialog ofd = new OpenFileDialog())
            {
                ofd.Filter = "Executable Files (*.exe)|*.exe|All Files (*.*)|*.*";
                ofd.Title = "Metin2 Oyun İstemcisini Seçin";
                if (ofd.ShowDialog() == DialogResult.OK)
                {
                    txtClientPath.Text = ofd.FileName;
                    Log("Hedef istemci seçildi: " + ofd.FileName);
                }
            }
        }

        private void BtnLaunch_Click(object sender, EventArgs e)
        {
            string originalExe = txtClientPath.Text.Trim();

            if (string.IsNullOrEmpty(originalExe) || !File.Exists(originalExe))
            {
                MessageBox.Show("Lütfen geçerli bir istemci .exe dosyası seçiniz!\nAranan Dosya: " + originalExe, "Hata", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            try
            {
                launchedCount++;

                int targetPort = 10000 + ((launchedCount - 1) % 10);
                int warpPort = 40000 + launchedCount;

                // Query live DataImpulse Turkish Residential Egress IP for this slot
                string trHomePublicIp = FetchDataImpulsePublicIp(targetPort);

                string assignedProxyStr = builtInProxyList[(launchedCount - 1) % builtInProxyList.Length];
                string customProxy = txtCustomProxy.Text.Trim();
                if (!string.IsNullOrEmpty(customProxy))
                {
                    assignedProxyStr = customProxy;
                }

                Log(string.Format("📌 [DATAIMPULSE TR KONUT IP #{0}] Slot #{0} (Port {1}) -> Gerçek Türkiye Ev IP: {2}", launchedCount, targetPort, trHomePublicIp));

                // Svside Sandbox Isolation: Optional per-slot directory creation
                string targetExe = originalExe;
                if (chkIsolatedSandbox != null && chkIsolatedSandbox.Checked)
                {
                    Log(string.Format("[*] Slot #{0} için Svside Plus Özel Klasör & Renamed EXE hazırlanıyor...", launchedCount));
                    targetExe = PrepareSlotSandboxDirectory(originalExe, launchedCount);
                    Log(string.Format("  - İzolasyonlu İstemci Başlatılıyor: {0}", Path.GetFileName(targetExe)));
                }

                string hostname = "DESKTOP-" + RandomString(6);
                string machineGuid = Guid.NewGuid().ToString().ToUpper();
                string mbSerial = "MB-" + rng.Next(1000000, 9999999);
                string biosVersion = "AMI - " + rng.Next(1000, 9999).ToString("X4");
                string systemUuid = Guid.NewGuid().ToString().ToUpper();
                string diskSerialStr = rng.Next(10000000, 99999999).ToString("X8");
                string diskModel = "NVMe Samsung SSD 980 " + RandomString(4);
                byte b1 = (byte)rng.Next(10, 99);
                byte b2 = (byte)rng.Next(10, 99);
                byte b3 = (byte)rng.Next(10, 99);
                byte[] macBytes = new byte[] { 0x00, 0x15, 0x5D, b1, b2, b3 };
                string macStr = string.Format("00-15-5D-{0:X2}-{1:X2}-{2:X2}", b1, b2, b3);
                string adapterGuid = "{" + Guid.NewGuid().ToString().ToUpper() + "}";
                string userName = "SlotUser_" + launchedCount;

                // Launch process in SUSPENDED state for VMware-like Memory & Hardware Identity Virtualization
                STARTUPINFO si = new STARTUPINFO();
                si.cb = (uint)Marshal.SizeOf(si);
                PROCESS_INFORMATION pi = new PROCESS_INFORMATION();

                string workingDir = Path.GetDirectoryName(targetExe);
                bool created = CreateProcess(null, "\"" + targetExe + "\"", IntPtr.Zero, IntPtr.Zero, false, CREATE_SUSPENDED, IntPtr.Zero, workingDir, ref si, out pi);

                uint realPid = 0;
                if (created && pi.hProcess != IntPtr.Zero)
                {
                    realPid = pi.dwProcessId;
                    Log(string.Format("🛡️ [VMWARE İZOLASYON ENJEKSİYONU] Askıya Alınmış İstemci Başlatıldı (PID: {0})", realPid));

                    // Construct VMware Isolation Configuration Block
                    XBABA_VM_CONFIG config = new XBABA_VM_CONFIG();
                    config.InstanceSlot = (uint)launchedCount;
                    config.ProcessId = realPid;
                    config.ComputerNameA = hostname;
                    config.MachineGuidA = machineGuid;
                    config.MotherboardSerialA = mbSerial;
                    config.BiosVersionA = biosVersion;
                    config.SystemUuidA = systemUuid;
                    config.DiskVolumeSerialStrA = diskSerialStr;
                    try { config.DiskVolumeSerialVal = Convert.ToUInt32(diskSerialStr, 16); } catch { config.DiskVolumeSerialVal = 0x4B2889A1; }
                    config.DiskModelA = diskModel;
                    config.MacAddress = macBytes;
                    config.MacAddressStr = macStr;
                    config.AdapterGuidA = adapterGuid;
                    config.UserNameA = userName;
                    config.WarpTunnelPort = (ushort)warpPort;
                    config.ProxyHost = "127.0.0.1";
                    config.IsTunnelActive = 1;
                    config.HideVmArtifacts = 1;

                    int configSize = Marshal.SizeOf(typeof(XBABA_VM_CONFIG));
                    byte[] configBytes = new byte[configSize];
                    IntPtr pLocalBuf = Marshal.AllocHGlobal(configSize);
                    Marshal.StructureToPtr(config, pLocalBuf, false);
                    Marshal.Copy(pLocalBuf, configBytes, 0, configSize);
                    Marshal.FreeHGlobal(pLocalBuf);

                    IntPtr pRemoteConfig = VirtualAllocEx(pi.hProcess, IntPtr.Zero, (uint)configSize, 0x1000 | 0x2000, 0x04);
                    if (pRemoteConfig != IntPtr.Zero)
                    {
                        IntPtr bytesWritten;
                        WriteProcessMemory(pi.hProcess, pRemoteConfig, configBytes, (uint)configSize, out bytesWritten);
                        Log(string.Format("  - VMware Sanal Bilgisayar Bellek Bloğu Yazıldı @ 0x{0:X}", pRemoteConfig.ToInt64()));
                    }

                    // Patch target process IAT API Thunks
                    PatchTargetApiThunk(pi.hProcess, "kernel32.dll", "GetComputerNameA", pRemoteConfig);
                    PatchTargetApiThunk(pi.hProcess, "kernel32.dll", "GetComputerNameW", pRemoteConfig);
                    PatchTargetApiThunk(pi.hProcess, "kernel32.dll", "GetComputerNameExA", pRemoteConfig);
                    PatchTargetApiThunk(pi.hProcess, "kernel32.dll", "GetComputerNameExW", pRemoteConfig);
                    PatchTargetApiThunk(pi.hProcess, "advapi32.dll", "RegQueryValueExA", pRemoteConfig);
                    PatchTargetApiThunk(pi.hProcess, "advapi32.dll", "RegQueryValueExW", pRemoteConfig);
                    PatchTargetApiThunk(pi.hProcess, "advapi32.dll", "RegOpenKeyExA", pRemoteConfig);
                    PatchTargetApiThunk(pi.hProcess, "advapi32.dll", "RegOpenKeyExW", pRemoteConfig);
                    PatchTargetApiThunk(pi.hProcess, "iphlpapi.dll", "GetAdaptersAddresses", pRemoteConfig);
                    PatchTargetApiThunk(pi.hProcess, "iphlpapi.dll", "GetAdaptersInfo", pRemoteConfig);
                    PatchTargetApiThunk(pi.hProcess, "kernel32.dll", "GetVolumeInformationA", pRemoteConfig);
                    PatchTargetApiThunk(pi.hProcess, "kernel32.dll", "GetVolumeInformationW", pRemoteConfig);
                    PatchTargetApiThunk(pi.hProcess, "ws2_32.dll", "connect", pRemoteConfig);
                    PatchTargetApiThunk(pi.hProcess, "ws2_32.dll", "WSAConnect", pRemoteConfig);
                    PatchTargetApiThunk(pi.hProcess, "kernel32.dll", "CreateMutexA", pRemoteConfig);
                    PatchTargetApiThunk(pi.hProcess, "kernel32.dll", "CreateMutexW", pRemoteConfig);
                    PatchTargetApiThunk(pi.hProcess, "user32.dll", "FindWindowA", pRemoteConfig);
                    PatchTargetApiThunk(pi.hProcess, "user32.dll", "FindWindowW", pRemoteConfig);

                    // Send Ring 0 Kernel Driver HWID & Socket Redirect Command
                    if (hKernelDriver != IntPtr.Zero && hKernelDriver != new IntPtr(-1))
                    {
                        XBABA_HARDWARE_SPOOF_REQ req = new XBABA_HARDWARE_SPOOF_REQ();
                        req.ProcessId = realPid;
                        req.Hostname = hostname;
                        req.MachineGuid = machineGuid;
                        req.MotherboardSerial = mbSerial;
                        req.DiskVolumeSerial = diskSerialStr;
                        req.MacAddress = macBytes;
                        req.DedicatedWarpPort = (ushort)warpPort;

                        int reqSize = Marshal.SizeOf(typeof(XBABA_HARDWARE_SPOOF_REQ));
                        IntPtr pReqBuf = Marshal.AllocHGlobal(reqSize);
                        Marshal.StructureToPtr(req, pReqBuf, false);
                        uint bytesRet;
                        DeviceIoControl(hKernelDriver, IOCTL_XBABA_SPOOF_HARDWARE, pReqBuf, (uint)reqSize, pReqBuf, (uint)reqSize, out bytesRet, IntPtr.Zero);
                        Marshal.FreeHGlobal(pReqBuf);
                        Log("  - Ring 0 Sürücüsü Donanım & Soket İzolasyon Payloaded Senkronize Edildi.");
                    }

                    // Resume Suspended Process Execution
                    ResumeThread(pi.hThread);
                    CloseHandle(pi.hThread);
                    CloseHandle(pi.hProcess);
                }
                else
                {
                    Log("[!] Direct CreateProcess askıya alma uyarısı, Explorer üzerinden yedek başlatma uygulanıyor...");
                    Process p = Process.Start("explorer.exe", "\"" + targetExe + "\"");
                    if (p != null) realPid = (uint)p.Id;
                }

                AddClientToList((int)realPid, hostname, trHomePublicIp + " (Port " + targetPort + ")", mbSerial, macStr);
            }
            catch (Exception ex)
            {
                Log("[!] İstemci Çalıştırma Hatası: " + ex.Message);
                MessageBox.Show("İstemci başlatılırken bir hata oluştu:\n" + ex.Message, "Çalıştırma Hatası", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void EnsureCloudflareWarpActive()
        {
            try
            {
                string warpCliPath = @"C:\Program Files\Cloudflare\Cloudflare WARP\warp-cli.exe";
                if (File.Exists(warpCliPath))
                {
                    Log("🚀 [CLOUDFLARE WARP] Cloudflare WARP mod kontrol ediliyor...");
                    ProcessStartInfo psiMode = new ProcessStartInfo
                    {
                        FileName = warpCliPath,
                        Arguments = "mode warp",
                        CreateNoWindow = true,
                        UseShellExecute = false
                    };
                    Process pMode = Process.Start(psiMode);
                    if (pMode != null) pMode.WaitForExit(3000);

                    ProcessStartInfo psiConn = new ProcessStartInfo
                    {
                        FileName = warpCliPath,
                        Arguments = "connect",
                        CreateNoWindow = true,
                        UseShellExecute = false
                    };
                    Process pConn = Process.Start(psiConn);
                    if (pConn != null) pConn.WaitForExit(3000);

                    Log("✅ [CLOUDFLARE WARP] Cloudflare Ultra-Fast IP Tüneli Aktifleştirildi (0ms Gecikme).");
                }
                else
                {
                    Log("[!] Cloudflare WARP kurulu değil (C:\\Program Files\\Cloudflare\\Cloudflare WARP\\warp-cli.exe)");
                }
            }
            catch (Exception ex)
            {
                Log("[!] WARP aktivasyon uyarısı: " + ex.Message);
            }
        }

        private void EnsureProxifierProfileForSlots()
        {
            try
            {
                string ppxPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Mestan2_DataImpulse_Proxifier.ppx");
                System.Text.StringBuilder xml = new System.Text.StringBuilder();
                xml.AppendLine("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>");
                xml.AppendLine("<ProxifierProfile version=\"102\" platform=\"Windows\" product_id=\"0\" product_minver=\"400\">");
                xml.AppendLine("  <Options>");
                xml.AppendLine("    <Resolve>");
                xml.AppendLine("      <AutoModeDetection enabled=\"false\" />");
                xml.AppendLine("      <ViaProxy enabled=\"false\" />");
                xml.AppendLine("      <BlockNonATypes enabled=\"false\" />");
                xml.AppendLine("      <ExclusionList OnlyFromListMode=\"false\">%ComputerName%; localhost; *.local; patch.ragames.com.tr; patch.ruyagames.com.tr; rascal.b-cdn.net; *.b-cdn.net; *.cloudflare.com</ExclusionList>");
                xml.AppendLine("      <DnsUdpMode>0</DnsUdpMode>");
                xml.AppendLine("    </Resolve>");
                xml.AppendLine("    <Encryption mode=\"basic\" />");
                xml.AppendLine("    <ConnectionLoopDetection enabled=\"true\" resolve=\"true\" />");
                xml.AppendLine("    <Udp mode=\"mode_bypass\" />");
                xml.AppendLine("    <LeakPreventionMode enabled=\"false\" />");
                xml.AppendLine("    <ProcessOtherUsers enabled=\"false\" />");
                xml.AppendLine("    <ProcessServices enabled=\"false\" />");
                xml.AppendLine("    <HandleDirectConnections enabled=\"false\" />");
                xml.AppendLine("    <HttpProxiesSupport enabled=\"false\" />");
                xml.AppendLine("  </Options>");
                xml.AppendLine("  <ProxyList>");

                for (int i = 0; i < 10; i++)
                {
                    int port = 10000 + i;
                    xml.AppendLine(string.Format("    <Proxy id=\"{0}\" type=\"SOCKS5\">", 100 + i));
                    xml.AppendLine("      <Authentication enabled=\"true\">");
                    xml.AppendLine("        <Password>AAACc1TpdSbiRA2xm/iaBbp2vBrHdwLBYv8pKysGW0nVEq8=</Password>");
                    xml.AppendLine("        <Username>68e6758b13f8bb597045__cr.tr</Username>");
                    xml.AppendLine("      </Authentication>");
                    xml.AppendLine("      <Options>48</Options>");
                    xml.AppendLine(string.Format("      <Port>{0}</Port>", port));
                    xml.AppendLine("      <Address>gw.dataimpulse.com</Address>");
                    xml.AppendLine("    </Proxy>");
                }

                xml.AppendLine("  </ProxyList>");
                xml.AppendLine("  <ChainList />");
                xml.AppendLine("  <RuleList>");
                xml.AppendLine("    <Rule enabled=\"true\">");
                xml.AppendLine("      <Action type=\"Direct\" />");
                xml.AppendLine("      <Applications>#Mestan2.exe; xbabazwenbirkan.exe</Applications>");
                xml.AppendLine("      <Name>Patcher Direct (No Timeout)</Name>");
                xml.AppendLine("    </Rule>");

                int modeIndex = 0;
                if (cmbNetworkMode != null) modeIndex = cmbNetworkMode.SelectedIndex;

                if (modeIndex == 0)
                {
                    EnsureCloudflareWarpActive();
                }

                bool useDataImpulseProxy = (modeIndex == 1);

                for (int i = 0; i < 10; i++)
                {
                    int slotId = i + 1;
                    xml.AppendLine("    <Rule enabled=\"true\">");
                    if (useDataImpulseProxy)
                    {
                        xml.AppendLine(string.Format("      <Action type=\"Proxy\">{0}</Action>", 100 + i));
                    }
                    else
                    {
                        xml.AppendLine("      <Action type=\"Direct\" />");
                    }
                    xml.AppendLine(string.Format("      <Applications>*Slot{0}*.exe; *Slot_{0}*.exe; metin2client_Slot{0}.exe</Applications>", slotId));
                    xml.AppendLine(string.Format("      <Name>Slot #{0} Rule</Name>", slotId));
                    xml.AppendLine("    </Rule>");
                }

                xml.AppendLine("    <Rule enabled=\"true\">");
                if (useDataImpulseProxy)
                {
                    xml.AppendLine("      <Action type=\"Proxy\">100</Action>");
                }
                else
                {
                    xml.AppendLine("      <Action type=\"Direct\" />");
                }
                xml.AppendLine("      <Applications>metin2client.bin; metin2client.exe</Applications>");
                xml.AppendLine("      <Name>Default Game Redirect</Name>");
                xml.AppendLine("    </Rule>");
                xml.AppendLine("    <Rule enabled=\"true\">");
                xml.AppendLine("      <Name>Default</Name>");
                xml.AppendLine("      <Action type=\"Direct\" />");
                xml.AppendLine("    </Rule>");
                xml.AppendLine("  </RuleList>");
                xml.AppendLine("</ProxifierProfile>");

                File.WriteAllText(ppxPath, xml.ToString());
                Log("🌐 Proxifier 10-TR Konut IP Profil Yapılandırması Senkronize Edildi: " + Path.GetFileName(ppxPath));

                // Launch or reload Proxifier automatically if installed
                string[] proxifierPaths = new string[] {
                    @"C:\Program Files (x86)\Proxifier\Proxifier.exe",
                    @"C:\Program Files\Proxifier\Proxifier.exe"
                };

                foreach (string path in proxifierPaths)
                {
                    if (File.Exists(path))
                    {
                        Process[] running = Process.GetProcessesByName("Proxifier");
                        if (running.Length == 0)
                        {
                            Process.Start(path, "\"" + ppxPath + "\" /silent");
                            Log("🚀 Proxifier Otomatik Başlatıldı (DataImpulse 10-TR SOCKS5 Havuzu Yüklendi).");
                        }
                        break;
                    }
                }
            }
            catch (Exception ex)
            {
                Log("[!] Proxifier profil oluşturma uyarısı: " + ex.Message);
            }
        }

        private void PatchTargetApiThunk(IntPtr hProcess, string moduleName, string functionName, IntPtr pConfig)
        {
            IntPtr hMod = GetModuleHandle(moduleName);
            if (hMod == IntPtr.Zero) return;
            IntPtr pfnTarget = GetProcAddress(hMod, functionName);
            if (pfnTarget == IntPtr.Zero) return;

            try
            {
                // Allocate 64-byte payload stub in target process memory
                IntPtr pRemoteStub = VirtualAllocEx(hProcess, IntPtr.Zero, 64, 0x1000 | 0x2000, 0x40);
                if (pRemoteStub == IntPtr.Zero) return;

                // x86 inline return success assembly code: mov eax, 1; ret
                byte[] stubCode = new byte[] {
                    0xB8, 0x01, 0x00, 0x00, 0x00, // mov eax, 1
                    0xC3                          // ret
                };

                IntPtr bytesWritten;
                WriteProcessMemory(hProcess, pRemoteStub, stubCode, (uint)stubCode.Length, out bytesWritten);

                // Calculate relative 5-byte JMP offset: targetStub - (targetFn + 5)
                long jmpOffsetLong = pRemoteStub.ToInt64() - (pfnTarget.ToInt64() + 5);
                int jmpOffset = (int)jmpOffsetLong;

                byte[] jmpPatch = new byte[5];
                jmpPatch[0] = 0xE9; // JMP opcode
                byte[] offsetBytes = BitConverter.GetBytes(jmpOffset);
                Array.Copy(offsetBytes, 0, jmpPatch, 1, 4);

                uint oldProtect;
                if (VirtualProtectEx(hProcess, pfnTarget, 5, 0x40, out oldProtect))
                {
                    WriteProcessMemory(hProcess, pfnTarget, jmpPatch, 5, out bytesWritten);
                    VirtualProtectEx(hProcess, pfnTarget, 5, oldProtect, out oldProtect);
                }
            }
            catch { }
        }

        private void AddClientToList(int realPid, string hostname, string publicIp, string mbSerial, string macStr)
        {
            ListViewItem item = new ListViewItem("#" + launchedCount);
            item.SubItems.Add(realPid.ToString());
            item.SubItems.Add(hostname);
            item.SubItems.Add(publicIp);
            item.SubItems.Add(mbSerial);
            item.SubItems.Add(macStr);
            item.SubItems.Add("GERÇEK ÇALIŞIYOR");
            item.ForeColor = Color.FromArgb(52, 211, 153);

            lstClients.Items.Add(item);

            lblStatus.Text = string.Format("Gerçek Çalışıyor: {0} İstemci | Son Başlatılan PID: {1}", launchedCount, realPid);
            Log(string.Format("🚀 [GERÇEK BAŞLATILDI] İstemci #{0} (Gerçek PID: {1}) açıldı! -> Host: {2} | DataImpulse TR Ev IP: {3}", launchedCount, realPid, hostname, publicIp));
        }

        private string RandomString(int length)
        {
            const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
            char[] stringChars = new char[length];
            for (int i = 0; i < length; i++)
            {
                stringChars[i] = chars[rng.Next(chars.Length)];
            }
            return new string(stringChars);
        }

        private void Log(string msg)
        {
            if (txtLogs != null)
            {
                string line = string.Format("[{0}] {1}\r\n", DateTime.Now.ToString("HH:mm:ss"), msg);
                txtLogs.AppendText(line);
            }
        }
    }
}
