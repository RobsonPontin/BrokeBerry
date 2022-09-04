using MQTT_Client.comm;
using System;

namespace MQTT_Client.tools
{
    class HostStorage
    {
        const string FILE_NAME = "location.xml";
        
        /// <summary>
        /// Load XML file
        /// </summary>
        /// <param name="hosts"> Ref to host object </param>
        public static Boolean LoadXML(ref ManualLocation manualLocation)
        {
            var xmlSer = new System.Xml.Serialization.XmlSerializer(manualLocation.GetType());

            try
            {
                using (var fs = new System.IO.FileStream(FILE_NAME, System.IO.FileMode.Open))
                {
                    manualLocation = (ManualLocation)xmlSer.Deserialize(fs);
                    return true;
                }
            }
            catch (Exception)
            {
                return false;
            }
        }

        /// <summary>
        /// Save XML File
        /// </summary>
        public static Boolean SaveXML(ManualLocation manualLocation)
        {
            if (manualLocation == null)
                return false;

            using (var xmlWriter = new System.Xml.XmlTextWriter(FILE_NAME, null))
            {
                xmlWriter.Formatting = System.Xml.Formatting.Indented;

                var xmlSer = new System.Xml.Serialization.XmlSerializer(manualLocation.GetType());

                xmlSer.Serialize(xmlWriter, manualLocation);
                xmlWriter.Flush();
                return true;
            }
        }
    }
}
