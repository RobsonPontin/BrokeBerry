using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MQTT_Client.comm
{
    /// <summary>
    /// Constains all info to access a location
    /// </summary>
    public class ManualLocation
    {
        public string IPAddress { get; set; }
        public int Port { get; set; }
        public string Name { get; set; }
    }
}
