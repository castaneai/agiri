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

        public TestClient Accept()
        {
            return new TestClient(listener.AcceptTcpClient());
        }
    }
}
