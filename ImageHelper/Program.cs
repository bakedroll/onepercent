using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;

namespace ImageHelper
{
    class Program
    {
        public static void ClearCurrentConsoleLine()
        {
            var currentLineCursor = Console.CursorTop;
            Console.SetCursorPosition(0, Console.CursorTop);
            Console.Write(new string(' ', Console.WindowWidth));
            Console.SetCursorPosition(0, currentLineCursor);
        }

        private static void Exit()
        {
            Console.Write("Press any key to exit... ");
            Console.ReadKey();
        }

        private static void Error(string message, string stackTrace = null)
        {
            Console.WriteLine("Error: {0}", message);

            if (stackTrace != null)
                Console.WriteLine("StackTrace: {0}", stackTrace);

            Exit();
        }

        private static void MergeImages(string redFile, string greenFile, string blueFile, string alphaFile, string resultFile)
        {
            var redBitmap = new Bitmap(redFile);
            var greenBitmap = new Bitmap(greenFile);
            var blueBitmap = new Bitmap(blueFile);
            var alphaBitmap = new Bitmap(alphaFile);

            var width = Math.Min(redBitmap.Width, greenBitmap.Width);
            var height = Math.Min(redBitmap.Height, greenBitmap.Height);

            var resultBitmap = new Bitmap(width, height, PixelFormat.Format32bppArgb);

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    resultBitmap.SetPixel(x, y, Color.FromArgb(
                        alphaBitmap.GetPixel(x, y).R,
                        redBitmap.GetPixel(x, y).R,
                        greenBitmap.GetPixel(x, y).G,
                        blueBitmap.GetPixel(x, y).B));
                }
            }

            resultBitmap.Save(resultFile, ImageFormat.Png);
        }

        private static void SubdivideImage(string filename, int cols, int rows, string outputdir)
        {
            var inputBitmap = new Bitmap(filename);

            var width = inputBitmap.Width;
            var height = inputBitmap.Height;

            if ((width % cols != 0) && (height % rows != 0))
            {
                return;
            }

            var pwidth = width / cols;
            var pheight = height / rows;

            for (int y = 0; y < rows; y++)
            {
                for (int x = 0; x < cols; x++)
                {
                    var outputBitmap = new Bitmap(pwidth, pheight, inputBitmap.PixelFormat);

                    for (int py = 0; py < pheight; py++)
                    {
                        for (int px = 0; px < pwidth; px++)
                        {
                            outputBitmap.SetPixel(px, py, inputBitmap.GetPixel(x*pwidth + px, y*pheight + py));
                        }
                    }

                    outputBitmap.Save(string.Format("{0}{1}x{2}.png", outputdir, x, y), ImageFormat.Png);
                }
            }
        }

        private static void ConvertStarsMap(string catalogFilename, string binFilename)
        {
            var catalogLines = File.ReadAllLines(catalogFilename);
            var binFile = File.Create(binFilename);
            using (var writer = new BinaryWriter(binFile))
            {

                writer.Write(catalogLines.Length);

                foreach (var cLine in catalogLines)
                {
                    var values = cLine.Split(' ').Where(x => !x.Equals(string.Empty)).ToArray();

                    writer.Write(float.Parse(values[3], CultureInfo.InvariantCulture));
                    writer.Write(float.Parse(values[4], CultureInfo.InvariantCulture));
                    writer.Write(float.Parse(values[5], CultureInfo.InvariantCulture));
                    writer.Write(float.Parse(values[2], CultureInfo.InvariantCulture));
                }

            }
            binFile.Close();
        }

        private static void ConvertCountriesMap(string tableFilename, string mapFilename, string greyscaleFilename, int newWidth, int newHeight, string binFilename)
        {
            var countriesBitmap = new Bitmap(mapFilename);

            Console.WriteLine("Reading table file {0}", tableFilename);

            var tableLines = File.ReadAllLines(tableFilename);

            Console.WriteLine("Writing bin file {0}", binFilename);

            var binFile = File.Create(binFilename);

            Console.WriteLine("Processing data...");

            var asen = new UTF8Encoding();

            var resultBitmap = new Bitmap(newWidth, newHeight, PixelFormat.Format32bppArgb);

            var countries = new Dictionary<Color, byte>();

            using (var writer = new BinaryWriter(binFile))
            {
                writer.Write(tableLines.Length);

                byte counter = 0;
                foreach (var tLine in tableLines)
                {
                    var values = tLine.Split('\t').Where(x => !x.Equals(string.Empty)).ToArray();

                    var color = ColorTranslator.FromHtml("#" + values[2]);

                    if (countries.ContainsKey(color))
                    {
                        Console.WriteLine("Warning: Color #{0} already exists ({1})", values[2], values[1]);
                    }
                    else
                    {
                        countries.Add(color, counter);
                    }

                    var encoded = asen.GetBytes(values[1]);

                    writer.Write(encoded.Length);
                    writer.Write(encoded);
                    writer.Write(float.Parse(values[3]));
                    writer.Write(int.Parse(values[4]));
                    writer.Write(counter);

                    counter++;
                }

                int oldWidth = countriesBitmap.Width;
                int oldHeight = countriesBitmap.Height;

                writer.Write(newWidth);
                writer.Write(newHeight);

                int percentCounter = 0;
                Console.WriteLine("0%");
                for (int y = 0; y < newHeight; y++)
                {
                    for (int x = 0; x < newWidth; x++)
                    {
                        var color = countriesBitmap.GetPixel((oldWidth * x) / newWidth, (oldHeight * y) / newHeight);

                        Color col;
                        byte id;

                        if (countries.ContainsKey(color))
                        {
                            countries.TryGetValue(color, out id);
                            col = Color.FromArgb(id, id, id);
                        }
                        else
                        {
                            col = Color.White;
                            id = 255;
                        }

                        writer.Write(id);
                        resultBitmap.SetPixel(x, y, col);
                    }

                    Console.SetCursorPosition(0, Console.CursorTop - 1);
                    ClearCurrentConsoleLine();

                    percentCounter++;
                    Console.WriteLine("{0}%", percentCounter * 100 / newHeight);
                }
            }

            Console.WriteLine("Saving {0}", greyscaleFilename);
            resultBitmap.Save(greyscaleFilename, ImageFormat.Png);
            binFile.Close();
        }

        static void Main(string[] args)
        {
            if (args.Length < 1)
            {
                Error("Not enough parameters");
                return;
            }

            try
            {
                var command = Int32.Parse(args[0]);
                switch (command)
                {
                    case 0:
                        MergeImages(args[1], args[2], args[3], args[4], args[5]);
                        break;
                    case 1:
                        SubdivideImage(args[1], Int32.Parse(args[2]), Int32.Parse(args[3]), args[4]);
                        break;
                    case 2:
                        ConvertStarsMap(args[1], args[2]);
                        break;
                    case 3:
                        ConvertCountriesMap(args[1], args[2], args[3], Int32.Parse(args[4]), Int32.Parse(args[5]), args[6]);
                        break;
                }
            }
            catch (Exception e)
            {
                var ex = e;
                string message = string.Empty;
                while (ex != null)
                {
                    message += string.Format("{0} ==> ", ex.Message);

                    ex = ex.InnerException;
                }


                Error(message, e.StackTrace);
                return;
            }

            Exit();
        }
    }
}
