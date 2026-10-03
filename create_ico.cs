using System;
using System.Drawing;
using System.IO;

class Program {
    static void Main() {
        using (Image img = Image.FromFile(@"c:\Users\yagiz\OneDrive\Desktop\m2\xbabazwenbirkan_logo.png"))
        using (Bitmap bmp = new Bitmap(img, new Size(64, 64))) {
            IntPtr hIcon = bmp.GetHicon();
            using (Icon icon = Icon.FromHandle(hIcon))
            using (FileStream fs = new FileStream(@"c:\Users\yagiz\OneDrive\Desktop\m2\xbabazwenbirkan.ico", FileMode.Create)) {
                icon.Save(fs);
            }
        }
        Console.WriteLine("ICO Icon Created Successfully!");
    }
}
