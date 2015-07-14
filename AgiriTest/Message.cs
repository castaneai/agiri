namespace AgiriTest
{
    public enum Command : byte
    {
        PingRequest = 0x00,
        PongResponse = 0x01,
        ListSocketRequest = 0x02,
        ListSocketResponse = 0x03,
        InjectOutgoingPacket = 0x04,
    }

    public sealed class Message
    {
        public Command Command
        {
            get { return command; }
        }

        public byte[] Data
        {
            get { return data; }
        }

        private readonly Command command;
        private readonly byte[] data;

        public Message(Command command, byte[] data = null)
        {
            this.command = command;
            this.data = data ?? new byte[0];
        }
    }
}
