using System.Net;
using System.Net.Sockets;

namespace AgiriTest
{
    public class TestServer
    {
        private readonly TcpListener listener;

        public TestServer(ushort listenPort)
        {
            listener = new TcpListener(new IPEndPoint(IPAddress.Loopback, listenPort));
            listener.Start();
        }

        public AgiriClient Accept()
        {
            return new AgiriClient(listener.AcceptTcpClient());
        }
    }
}
