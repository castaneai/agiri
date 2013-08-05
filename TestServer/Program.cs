using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Net;
using System.Net.Sockets;

namespace TestServer
{
    class Program
    {
        static void Main(string[] args)
        {
            var server = new Server(10000);

            while (true) {
                Console.WriteLine("waiting client..");
                var client = server.Accept();
                Console.WriteLine("accepted client.");
                try {
                    while (true) {
                        var request = client.Receive();
                        Console.WriteLine("[received message]{0}", request);

                        var response = request.ToUpper();
                        client.Send(response);
                    }
                }
                catch {
                    Console.WriteLine("client error. disconnected.");
                }
            }
        }
    }
}
