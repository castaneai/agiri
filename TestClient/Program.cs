using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;

namespace TestClient
{
    class Program
    {
        static void Main(string[] args)
        {
            LoadAgiri();

            var client = new Client(10000);
            Console.WriteLine("client connected. waiting...");
            Console.ReadKey();
        }

        #region loadLibrary agiri.dll

        [DllImport("kernel32", SetLastError = true, CharSet = CharSet.Unicode)]
        static extern IntPtr LoadLibrary(string lpFileName);

        static bool LoadAgiri()
        {
            var handle = LoadLibrary("agiri.dll");
            return handle != IntPtr.Zero;
        }

        #endregion
    }
}
