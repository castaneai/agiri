using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Net;
using System.Net.Sockets;

namespace TestClient
{
    public class Client
    {
        public Client(ushort serverPort)
        {
            tcpClient = new TcpClient();
            tcpClient.Connect(new IPEndPoint(IPAddress.Loopback, serverPort));
            reader = new StreamReader(tcpClient.GetStream());
            writer = new StreamWriter(tcpClient.GetStream());
        }

        public Client(TcpClient baseClient)
        {
            tcpClient = baseClient;
            reader = new StreamReader(tcpClient.GetStream());
            writer = new StreamWriter(tcpClient.GetStream());
        }

        public void Send(string message)
        {
            writer.WriteLine(message);
            writer.Flush();
        }

        public string Receive()
        {
            var response = reader.ReadLine();
            return response;
        }

        #region private

        private TcpClient tcpClient;

        private StreamReader reader;

        private StreamWriter writer;

        #endregion
    }
}
