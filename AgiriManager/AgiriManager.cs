using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;

namespace AgiriManager
{
    public class AgiriManager
    {
        private UdpClient client;

        public AgiriManager(ushort agiriPort)
        {
            var agiriEP = new IPEndPoint(IPAddress.Loopback, agiriPort);
            client = new UdpClient();
            client.Connect(agiriEP);
        }

        public void Send(string message)
        {
            var bytes = Encoding.ASCII.GetBytes(message);
            client.Send(bytes, bytes.Length);
        }

        public string Receive()
        {
            IPEndPoint peerEP = null;
            var receivedBytes = client.Receive(ref peerEP);
            return Encoding.ASCII.GetString(receivedBytes);
        }
    }
}
