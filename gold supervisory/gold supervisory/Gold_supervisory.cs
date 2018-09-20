using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;
using System.Threading;

namespace lab2
{
    class Program
    {
        static String loopNum = "";
        static String command = "";
        //store state
        static String[] gantries = new String[3] { "--", "--", "--" }; //gantries state array
        static String[] buggies = new String[2] { "running", "waiting" }; //buggies state array
        static int count = 1;
        static int loop = 0;


        static void Main(string[] args)
        {
            SerialPort port = new SerialPort();
            port.PortName = "COM17";
            port.BaudRate = 9600;
            port.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);
            port.Open();



            //set up  xbee for communication 
            port.Write("+++");
            Thread.Sleep(1100);
            port.WriteLine("ATID 3111, CH C, CN");


            //ask for loop wanted
            Console.Write("Please input the loop number you want:");
            loopNum = Console.ReadLine(); //take in number of loops
            int loopNumber = Int32.Parse(loopNum); //parse string to integer
            loop = loopNumber;
            command = "run1"; //start buggy 1

            while (true)
            {
                if (command != "")
                {
                    port.WriteLine(command); //send commands to the buggies from WriteState function
                    command = ""; //clear command
                }

            }


        }

        private static void DataReceivedHandler(
                            object sender,
                            SerialDataReceivedEventArgs e)
        {

            SerialPort sp = (SerialPort)sender;
            string indata = sp.ReadLine();

            Console.WriteLine(indata);
            //store state

            WriteState(indata);
            //display


        }

        private static void PrintState() //prints out arrays storing the state of the gantries and state of the buggies
        {
            Console.WriteLine("---------------------------------------------------");
            Console.WriteLine("G1:" + gantries[0]);
            Console.WriteLine("G2:" + gantries[1]);
            Console.WriteLine("G3:" + gantries[2]);
            Console.WriteLine("B1:" + buggies[0]);
            Console.WriteLine("B2:" + buggies[1]);
        }


        private static void WriteState(string indata)
        {
            if (indata == "B1G1\r")
            {
                gantries[0] = "Buggy 1"; //set gantry array to hold location of buggy 1
                Thread.Sleep(500); //pause program
                command = "run1"; //run buggy 1
                buggies[0] = "leaving gantry 1"; //set buggy array to hold state of buggy 1
                PrintState(); //print state of the track
            
                //clear arrays
                gantries[0] = "--";
                buggies[0] = "";

            }
            else if (indata == "B1G2\r")
            {
                if (count == loop)
                { //if counter and loop number are equal park buggy 1
                    command = "park1";
                }
                else {
                    gantries[1] = "Buggy 1"; //set gantry array to hold location of buggy 1
                    Thread.Sleep(500); //pause program
                    command = "run1"; //run buggy 1
                    buggies[0] = "leaving gantry 2"; //set buggy array to hold state of buggy 1
                    PrintState(); //print state of the track

                    //clear arrays
                    gantries[1] = "--";
                    buggies[0] = "--";
                }
            }

            else if (indata == "B1G3\r")
            {
                count++; //add to counter of loops
                gantries[2] = "Buggy 1"; //set gantry array to hold location of buggy 1
                //clear arrays
                gantries[1] = "--";
                buggies[0] = "--";
                Thread.Sleep(500); //pause program
                command = "run2"; //run buggy 2
                buggies[0] = "stopped at gantry 3, waiting"; //set buggy array to hold state of buggy 1 and 2
                buggies[1] = "running";
                PrintState(); //print state of the track
                buggies[1] = ""; //clear array
            }

            else if (indata == "B2G2\r") { 
            
                gantries[1] = "Buggy 2"; //set gantry array to hold location of buggy 2
                Thread.Sleep(500); //pause program
                command = "run2"; //run buggy 2 
                buggies[1] = "leaving gantry 2";  //set buggy array to hold state of buggy 2
                PrintState(); //print state of the track
                gantries[1] = "--";
                buggies[1] = "running";

            }

            else if (indata == "B2G1\r")
            {
                gantries[0] = "Buggy 2"; //set gantry array to hold location of buggy 2
                Thread.Sleep(500);
                command = "run2";
                buggies[0] = "leaving gantry 1";
                //print state of the track
                PrintState();
                //clear array
                gantries[0] = "--";
                buggies[0] = "running";
            }

            else if (indata == "PARKING BUGGY 2\r")
            {
                //set arrays to hold correct loction of buggies
                buggies[1] = "parking";
                buggies[0] = "leaving gantry 3";
                gantries[2] = "Buggy 1";
                gantries[0] = "--";
                gantries[1] = "--";
                //print state of the track
                PrintState();
                command = "run1"; //run buggy 2

                buggies[0] = "running";
                gantries[2] = "--";
            }

            else if (indata == "PARKING BUGGY 1\r")
            {
                command = ""; //clear command
                //set arrays to hold correct loction of buggies
                buggies[0] = "parking";
                buggies[1] = "parked";
                //clear arrays
                gantries[0] = "--";
                gantries[1] = "--";
                gantries[2] = "--";
                //print state of the track
                PrintState();
            }


        }
    }
}

