// Default URL for triggering event grid function in the local environment.
// http://localhost:7071/runtime/webhooks/EventGrid?functionName={functionname}
using System;
using Azure;
using System.Net.Http;
using Azure.Core.Pipeline;
using Azure.DigitalTwins.Core;
using Azure.Identity;
using Azure.Messaging.EventGrid;
using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Extensions.EventGrid;
using Microsoft.Extensions.Logging;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace AA_DTHubtoTwins
{
    public class AA_HubtoTwins
    {
        private static readonly string adtUrl = Environment.GetEnvironmentVariable("ADT_SERVICE_URL");
        private static readonly HttpClient shttpClient = new HttpClient();

        [FunctionName("IoTHubtoTwins")]
        public async static void Run([EventGridTrigger] EventGridEvent eventGridEvent, ILogger log)
        {
            if (adtUrl == null) log.LogError("Application setting \"ADT_SERVICE_URL\" not set");

            try
            {
                // Authenticate with Digital Twins
                var cred = new DefaultAzureCredential();
                var client = new DigitalTwinsClient(
                new Uri(adtUrl),
                cred,
                new DigitalTwinsClientOptions
                {
                    Transport = new HttpClientTransport(shttpClient)
                });
                log.LogInformation($"ADT service client connection created.");

                if (eventGridEvent != null && eventGridEvent.Data != null)
                {
                    log.LogInformation(eventGridEvent.Data.ToString());

                    JObject deviceMessage = (JObject)JsonConvert.DeserializeObject(eventGridEvent.Data.ToString());
                    string deviceId = (string)deviceMessage["systemProperties"]["connectionDeviceId"];
                    var temperature = deviceMessage["body"]["Temperature"];
                    var humidity = deviceMessage["body"]["Humidity"];
                    var X = deviceMessage["body"]["X"];
                    var Y = deviceMessage["body"]["Y"];
                    var Z = deviceMessage["body"]["Z"];

                    log.LogInformation($"Device:{deviceId} Temperature is:{temperature} Humidity is:{humidity} X is:{X} Y is:{Y} Z is:{Z}" );

                    var updateTwinData = new JsonPatchDocument();
                    updateTwinData.AppendReplace("/Temperature", temperature.Value<double>());
                    updateTwinData.AppendReplace("/Humidity", humidity.Value<double>());
                    updateTwinData.AppendReplace("/X", X.Value<double>());
                    updateTwinData.AppendReplace("/Y", Y.Value<double>());
                    updateTwinData.AppendReplace("/Z", Z.Value<double>());
                    await client.UpdateDigitalTwinAsync(deviceId, updateTwinData);
                }
            }
            catch (Exception ex)
            {
                log.LogError($"Error in ingest function: {ex.Message}");
            }
        }
    }
}