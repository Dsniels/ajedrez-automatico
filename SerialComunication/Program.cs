using Microsoft.Extensions.Logging;
using System.IO.Ports;



using var factory = LoggerFactory.Create(builder => builder.AddConsole());
ILogger logger = factory.CreateLogger("Program");

var serialCam = new SerialPort("COM4", 9600);
var serialArduino = new SerialPort("COM3", 9600);
TryToConnect(serialCam);
TryToConnect(serialArduino);


#region Background Task
_ = Task.Run(async () =>
{
    while (true)
    {
        try
        {
            string photoFlag = serialCam.ReadLine().Trim();
            logger.LogInformation("Camera: {}", photoFlag);
            serialArduino.WriteLine(photoFlag);
            await Task.Delay(100);
        }
        catch (IOException) { }
        catch (Exception ex)
        {
            logger.LogWarning("{}", ex.Message);
            if (ex.Message.Contains("port is closed"))
            {
                TryToConnect(serialArduino);
                TryToConnect(serialCam);
                await Task.Delay(1000);
            }
        }


    }
});
#endregion

#region Main 
while (true)
{
    try
    {
        string flag = serialArduino.ReadLine().Trim();
        if (flag == "1")
        {
            logger.LogInformation("Arduino: {}", flag);
            serialCam.WriteLine("TAKE");
        }
    }
    catch (IOException)
    { }
    catch (Exception ex)
    {
        logger.LogWarning("{}", ex.Message);
        if (ex.Message.Contains("port is closed"))
        {
            TryToConnect(serialArduino);
            TryToConnect(serialCam);
            await Task.Delay(1000);
        }
    }

    await Task.Delay(100);
}
#endregion 


#region  Connect SerialPort
void TryToConnect(SerialPort serialPort)
{
    try
    {
        if (!serialPort.IsOpen)
        {
            serialPort.Open();
        }

    }
    catch (System.Exception) { }
}
#endregion
