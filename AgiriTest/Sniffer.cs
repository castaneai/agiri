using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Net;
using System.Net.Sockets;

namespace AgiriTest 
{
    public class SocketInfo
    {
        public int SocketID { get; set; }
        public IPEndPoint EndPoint { get; set; }
    }

    public sealed class NinjaConnection : IDisposable
    {
        private readonly TcpClient tcpClient;
        private readonly BinaryReader reader;
        private readonly BinaryWriter writer;

        public NinjaConnection(ushort serverPort)
        {
            var serverEP = new IPEndPoint(IPAddress.Loopback, serverPort);
            tcpClient = new TcpClient();
            tcpClient.Connect(serverEP);
            reader = new BinaryReader(tcpClient.GetStream());
            writer = new BinaryWriter(tcpClient.GetStream());
        }

        public NinjaConnection(TcpClient baseClient)
        {
            tcpClient = baseClient;
            reader = new BinaryReader(tcpClient.GetStream());
            writer = new BinaryWriter(tcpClient.GetStream());
        }

        public void Send(Message message)
        {
            writer.Write((byte)message.Command);
            writer.Write(BitConverter.GetBytes((Int32)message.Data.Length));
            writer.Write(message.Data);
            writer.Flush();
        }

        public Message Receive()
        {
            var command = (Command)reader.ReadByte();
            var length = reader.ReadInt32();
            var data = reader.ReadBytes(length);
            return new Message(command, data);
        }

        public void Close()
        {
            reader.Close();
            writer.Close();
        }

        public IList<SocketInfo> GetAllSockets()
        {
            var result = new List<SocketInfo>();
            var request = new Message(Command.ListSocketRequest);
            Send(request);
            var response = Receive();
            var reader = new BinaryReader(new MemoryStream(response.Data));
            var socketCount = reader.ReadInt32();
            for (var i = 0; i < socketCount; i++) {
                var socketID = reader.ReadInt32();
                var ip = new IPAddress(reader.ReadInt32());
                var port = reader.ReadUInt16();
                result.Add(new SocketInfo { SocketID = socketID, EndPoint = new IPEndPoint(ip, port) });
            }
            return result;
        }

        public void InjectOutgoingPacket(int targetSocketID, byte[] packetData)
        {
            var data = BitConverter.GetBytes(targetSocketID).Concat(BitConverter.GetBytes(packetData.Length)).Concat(packetData).ToArray();
            var request = new Message(Command.InjectOutgoingPacket, data);
            Send(request);
        }

        private void switchSniffIncomingPacket(int targetSocketID, bool state)
        {
            var data = BitConverter.GetBytes(targetSocketID).Concat(
                BitConverter.GetBytes((byte)(state ? 1 : 0))).ToArray();
            var request = new Message(Command.SwitchSniffIncomingPacketRequest, data);
            Send(request);
        }

        public void StartSniffIncomingPacket(int targetSocketID)
        {
            switchSniffIncomingPacket(targetSocketID, true);
        }

        public void StopSniffIncomingPacket(int targetSocketID)
        {
            switchSniffIncomingPacket(targetSocketID, false);
        }

        public byte[] ReceiveSniffedIncomingPacket(int targetSocketID)
        {
            while (true) {
                var mes = Receive();
                if (mes.Command != Command.IncomingPacketResponse) continue;
                if (BitConverter.ToUInt32(mes.Data, 0) != targetSocketID) continue;
                var packetSize = BitConverter.ToInt32(mes.Data, 4);
                return mes.Data.Skip(8).Take(packetSize).ToArray();
            }
        }

        #region IDisposable Support
        private bool disposedValue = false; // To detect redundant calls

        private void Dispose(bool disposing)
        {
            if (!disposedValue) {
                if (disposing) {
                    reader.Dispose();
                    writer.Dispose();
                    tcpClient.Close();
                }
                disposedValue = true;
            }
        }

        public void Dispose()
        {
            // Do not change this code. Put cleanup code in Dispose(bool disposing) above.
            Dispose(true);
        }
        #endregion
    }
}
