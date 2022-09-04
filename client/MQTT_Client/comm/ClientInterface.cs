using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading.Tasks;
using uPLibrary.Networking.M2Mqtt;
using uPLibrary.Networking.M2Mqtt.Messages;

/*
 * Class holds the protocol and client to host connection
 */

namespace MQTT_Client.comm
{
    enum MESSAGE_TYPE
    {
        MSG_GET = 0,
        MSG_SET = 1
    }

    enum COMMAND_LIST
    {
        SET_DIO = 20,
        SET_AIO = 21,
        READ_DIO = 30,
        STOP_REASON = 40,
    }

    enum STOP_REASON
    {
        ABASTECIMENTO_ARAME = 10,
        CONSERTO_TELA,
        ROLO_DEFEITO,
        MANUTENCAO,
        AJUSTE_MAQUINA,
        INTERVALO,
        REASON_MAX
    }
    
    class ClientInterface
    {
        private const string PRE_AMBLE = "~~";
        private const string POST_AMBLE = "\r\n";

        const string user = "root";
        const string passsword = "toor";

        const string PUBLISH_TOPIC = "/dev/mosq_subscriber_client";

        const int MAX_DIO_PINS = 40;
        const int KEEP_ALIVE_CLIENT = 15; // in seconds
        
        MqttClient mqqtClient;

        // Subscribe only to the topic that will give feedback from the raspberry operations
        private string[] _topics = { "/dev/mosq_publish_client" };

        public event EventHandler<MQQTPublishEventArgs> MQQTPublishReceived;
        public event EventHandler<MQQTPublishedEventArgs> MQQTPublished;
        public event EventHandler<MQQTConnectionEventArgs> MQQTConnected;
        public event EventHandler<MQQTConnectionEventArgs> MQQTDisconnected;
        public event EventHandler<MQQTSubscribeEventArgs> MQQTSubscribed;

        protected virtual void OnMQQTPublish(MQQTPublishEventArgs e)
        {
            MQQTPublishReceived?.Invoke(null, e);
        }

        protected virtual void OnMQQTPublished(MQQTPublishedEventArgs e)
        {
            MQQTPublished?.Invoke(null, e);
        }

        protected virtual void OnMQQTConnection(MQQTConnectionEventArgs e)
        {
            MQQTConnected?.Invoke(null, e);
        }

        protected virtual void OnMQQTDisconnection(MQQTConnectionEventArgs e)
        {
            MQQTDisconnected?.Invoke(null, e);
        }

        protected virtual void OnMQQTSubscribe(MQQTSubscribeEventArgs e)
        {
            MQQTSubscribed?.Invoke(null, e);
        }

        public ClientInterface(string name, string ip, int port)
        {
            this.Name = name;
            this.IP = ip;
            this.Port = port;
        }

        public string Name { get; set; }
        public string IP { get; set; }
        public int Port { get; set; }

        /// <summary>
        /// Try to connect to the host
        /// </summary>
        public bool Connect()
        {
            int returnCode = -1;
            string clientID;
            IPAddress iPAddress;
            MQQTConnectionEventArgs ev = new MQQTConnectionEventArgs("", "", false, "", false);

            if (IPAddress.TryParse(this.IP, out iPAddress) == false)
                return false;

            if (this.Port <= 0)
                return false;

            clientID = Guid.NewGuid().ToString();

            mqqtClient = new MqttClient(IPAddress.Parse(this.IP), this.Port, false, null, null, MqttSslProtocols.None);

            /*
             * clientID - 
             * user - 
             * password - 
             * cleanSession - 
             * keepAlivePeriod - in seconds
             */
            try
            {
                returnCode = mqqtClient.Connect(clientID, user, passsword, true, KEEP_ALIVE_CLIENT);
            }
            catch (Exception e)
            {
                return false;
            }

            switch (returnCode)
            {
                case MqttMsgConnack.CONN_ACCEPTED:
                    break;

                case MqttMsgConnack.CONN_REFUSED_IDENT_REJECTED:
                case MqttMsgConnack.CONN_REFUSED_NOT_AUTHORIZED:
                case MqttMsgConnack.CONN_REFUSED_PROT_VERS:
                case MqttMsgConnack.CONN_REFUSED_SERVER_UNAVAILABLE:
                case MqttMsgConnack.CONN_REFUSED_USERNAME_PASSWORD:
                    return false;
            }

            if (returnCode == MqttMsgConnack.CONN_ACCEPTED)
            {
                ev.IsConnected = true;
                OnMQQTConnection(ev);
            }

            mqqtClient.MqttMsgSubscribed += Client_MqttMsgSubscribed;
            mqqtClient.MqttMsgUnsubscribed += MqqtClient_MqttMsgUnsubscribed;
            mqqtClient.MqttMsgPublishReceived += Client_MqttMsgPublishReceived;
            mqqtClient.ConnectionClosed += Client_ConnectionClosed;
            mqqtClient.MqttMsgPublished += MqqtClient_MqttMsgPublished;

            return true;
        }

        public bool Disconnect()
        {
            mqqtClient.Disconnect();

            mqqtClient.MqttMsgSubscribed -= Client_MqttMsgSubscribed;
            mqqtClient.MqttMsgUnsubscribed -= MqqtClient_MqttMsgUnsubscribed;
            mqqtClient.MqttMsgPublishReceived -= Client_MqttMsgPublishReceived;
            mqqtClient.ConnectionClosed -= Client_ConnectionClosed;
            mqqtClient.MqttMsgPublished -= MqqtClient_MqttMsgPublished;

            return true;
        }

        public void Subscribe()
        {
            if (mqqtClient == null)
                return;

            if (_topics.Length == 0)
                return;

            /* NOTE: QOS levels must be same length of topics */
            var qos_levels = new byte[_topics.Length];
            for (int i = 0; i < _topics.Length; i++)
            {
                qos_levels[i] = MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE;
            }

            if (mqqtClient.IsConnected)
            {
                mqqtClient.Subscribe(_topics, qos_levels);
            }
        }

        public void Unsubscribe()
        {
            if (mqqtClient == null)
                return;

            mqqtClient.Unsubscribe(_topics);
        }

        public void PublishMessage(byte[] msg)
        {
            if (msg == null || msg.Length == 0)
                return;

            mqqtClient.Publish(_topics[0], msg, MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE, false);
        }

        public bool PublishMessage(string msg)
        {
            if (mqqtClient == null)
                return false;

            if (msg == null)
                return false;

            byte[] msgInBytes = Encoding.ASCII.GetBytes(msg);

            if (mqqtClient.Publish(PUBLISH_TOPIC, msgInBytes) == 0)
                return false;

            return true;
        }

        public bool SetResetDIO(int pin, bool level)
        {
            if ((pin < 0) || (pin > MAX_DIO_PINS))
                return false;

            return PublishMessage(EncodeMessage("", MESSAGE_TYPE.MSG_SET, COMMAND_LIST.SET_DIO, pin, level));
        }

        public bool GetDIOStatus(int pin_id)
        {
            if ((pin_id < 0) || (pin_id > MAX_DIO_PINS))
                return false;

            return PublishMessage(EncodeMessage("", MESSAGE_TYPE.MSG_GET, COMMAND_LIST.READ_DIO, pin_id));
        }


        /// <summary>
        /// Read a specific input or 40 to read all setup inputs
        /// </summary>
        /// <param name="pin"></param>
        /// <returns></returns>
        public bool ReadInput(int pin)
        {
            if ((pin < 0) || (pin > MAX_DIO_PINS))
                return false;

            return PublishMessage(EncodeMessage("", MESSAGE_TYPE.MSG_GET, COMMAND_LIST.READ_DIO, pin, false));
        }


        public bool SendStopReason(STOP_REASON reason, string message)
        {
            if ((reason < STOP_REASON.ABASTECIMENTO_ARAME) || (reason >= STOP_REASON.REASON_MAX))
                return false;

            return PublishMessage(EncodeMessage(message, MESSAGE_TYPE.MSG_SET, COMMAND_LIST.STOP_REASON, (int)reason, false));
        }


        /// <summary>
        /// Prepare message to be sent
        /// TODO: reorganize protocol in a way no standard msg is necessary and consider shift, product, and machine id
        /// </summary>
        /// <param name="message"> random msg </param>
        /// <param name="type"> Get, Set, ... </param>
        /// <param name="cmd"> Command type </param>
        /// <param name="id"> Pin OR Stop Reason Number depeding on command type</param>
        /// <param name="level">True/False High/Low for GPIO control</param>
        /// <returns></returns>
        private string EncodeMessage(string message, MESSAGE_TYPE type, COMMAND_LIST cmd, int id, bool level = false)
        {           
            if (id > MAX_DIO_PINS)
                return null;

            if ((id < 0) || (string.IsNullOrEmpty(message)))
            {
                return string.Concat(PRE_AMBLE, (int)type, ((int)cmd).ToString("D2"), id.ToString("D2"), Convert.ToInt16(level), "default_msg", POST_AMBLE);
            }

            return string.Concat(PRE_AMBLE, (int)type, ((int)cmd).ToString("D2"), id.ToString("D2"), Convert.ToInt16(level), message, POST_AMBLE);
        }

 
        private bool SubscribeToTopic(string topic)
        {
            int messageID = 0;

            if (string.IsNullOrEmpty(topic))
                return false;
                        
            if (mqqtClient.IsConnected)
            {
                messageID = mqqtClient.Subscribe(_topics, new byte[] { MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE });
            }

            if (messageID != 0)
                return true;

            return false;
        }

        /// <summary>
        /// Return all registered topics
        /// </summary>
        public string[] Topics
        {
            get { return _topics; }
        }

        /// <summary>
        /// Is the client connected to the host?
        /// </summary>
        public bool IsConnected
        {
            get
            {
                if (mqqtClient == null)
                    return false;

                return mqqtClient.IsConnected;
            }
        }

        private void Client_ConnectionClosed(object sender, EventArgs e)
        {   
            MQQTConnectionEventArgs MQQTEvent = new MQQTConnectionEventArgs(mqqtClient.ClientId, null, mqqtClient.IsConnected, mqqtClient.WillTopic, false);
            OnMQQTDisconnection(MQQTEvent);
        }

        private void Client_MqttMsgPublishReceived(object sender, uPLibrary.Networking.M2Mqtt.Messages.MqttMsgPublishEventArgs e)
        {            
            MQQTPublishEventArgs MQQTEvent = new MQQTPublishEventArgs(mqqtClient.ClientId, null, e.Topic, ByteToString(e.Message));
            OnMQQTPublish(MQQTEvent);
        }

        private void MqqtClient_MqttMsgPublished(object sender, MqttMsgPublishedEventArgs e)
        {
            MQQTPublishedEventArgs MQQTEvent = new MQQTPublishedEventArgs(mqqtClient.ClientId, null, mqqtClient.WillTopic, e.IsPublished, e.MessageId);
            OnMQQTPublished(MQQTEvent);
        }

        private void Client_MqttMsgSubscribed(object sender, uPLibrary.Networking.M2Mqtt.Messages.MqttMsgSubscribedEventArgs e)
        {
            MQQTSubscribeEventArgs MQQTEvent = new MQQTSubscribeEventArgs(mqqtClient.ClientId, null, mqqtClient.WillTopic, true);
            OnMQQTSubscribe(MQQTEvent);
        }

        private void MqqtClient_MqttMsgUnsubscribed(object sender, uPLibrary.Networking.M2Mqtt.Messages.MqttMsgUnsubscribedEventArgs e)
        {
            MQQTSubscribeEventArgs MQQTEvent = new MQQTSubscribeEventArgs(mqqtClient.ClientId, null, mqqtClient.WillTopic, false);
            //TODO: fix it OnMQQTPublish(MQQTEvent);
        }
        
        public static string ByteToString(byte[] message)
        {
            return Encoding.ASCII.GetString(message, 0, message.Length); ;
        }
    }

    public class MQQTConnectionEventArgs : MQQTEventArgs
    {
        private bool _connectionStatus;

        public MQQTConnectionEventArgs(string hostName, string hostIP, bool isConnected, string topicName, bool ConnectionStatus) : base(hostName, hostIP, topicName)
        {
            _connectionStatus = ConnectionStatus;
        }

        public bool IsConnected
        {
            get { return _connectionStatus; }
            set { _connectionStatus = value; }
        }
    }

    public class MQQTSubscribeEventArgs : MQQTEventArgs
    {
        private bool _subscribed;

        public MQQTSubscribeEventArgs(string client_id, string hostIP, string topicName, bool subscribed) : base(client_id, hostIP, topicName)
        {
            _subscribed = subscribed;
        }

        public bool IsSubscribed
        {
            get { return _subscribed; }
        }
    }

    public class MQQTPublishEventArgs : MQQTEventArgs
    {
        private string _message;

        public MQQTPublishEventArgs(string hostName, string hostIP, string topicName, string message) : base(hostName, hostIP, topicName)
        {
            _message = message;
        }

        public string Message
        {
            get { return _message; }
        }
    }

    public class MQQTPublishedEventArgs : MQQTEventArgs
    {
        private int _messageID;
        private bool _isPublished;

        public MQQTPublishedEventArgs(string hostName, string hostIP, string topicName, bool isPublished, int messageID) : base(hostName, hostIP, topicName)
        {
            _messageID = messageID;
            _isPublished = isPublished;
        }

        public int MessageID { get { return _messageID; } }
        public bool IsPublished { get { return _isPublished; } }
    }

    public abstract class MQQTEventArgs : EventArgs
    {
        private string _hostName;
        private string _hostIP;
        private string _topicName;
        
        public MQQTEventArgs(string hostName, string hostIP, string topicName)
        {
            _hostName = hostName;
            _hostIP = hostIP;
            _topicName = topicName;
        }

        public string Name
        {
            get { return _hostName; }
        }

        public string IP
        {
            get { return _hostIP; }
        }

        public string TopicName
        {
            get { return _topicName; }
        }
    }
}
