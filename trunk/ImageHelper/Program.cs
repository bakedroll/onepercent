using System;
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
        private static void Exit()
        {
            Console.Write("Press any key to exit... ");
            Console.ReadKey();
        }

        private static void Error(string message)
        {
            Console.WriteLine("Error: {0}", message);
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

        private static void ConvertCountriesMap(string tableFilename, string mapFilename, string binFilename)
        {
            var countriesBitmap = new Bitmap(mapFilename);

            var tableLines = File.ReadAllLines(tableFilename);
            var binFile = File.Create(binFilename);

            var asen = new UTF8Encoding();

            using (var writer = new BinaryWriter(binFile))
            {

                writer.Write(tableLines.Length);

                foreach (var tLine in tableLines)
                {
                    var values = tLine.Split('\t').Where(x => !x.Equals(string.Empty)).ToArray();

                    var encoded = asen.GetBytes(values[1]);
                    var color = ColorTranslator.FromHtml("#" + values[2]);

                    writer.Write(encoded.Length);
                    writer.Write(encoded);
                    writer.Write(float.Parse(values[3]));
                    writer.Write(int.Parse(values[4]));
                    writer.Write(color.R);
                    writer.Write(color.G);
                    writer.Write(color.B);
                }

                writer.Write(countriesBitmap.Width);
                writer.Write(countriesBitmap.Height);

                for (int y = 0; y < countriesBitmap.Height; y++)
                {
                    for (int x = 0; x < countriesBitmap.Width; x++)
                    {
                        var color = countriesBitmap.GetPixel(x, y);

                        writer.Write(color.R);
                        writer.Write(color.G);
                        writer.Write(color.B);
                    }
                }
            }
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
                        ConvertCountriesMap(args[1], args[2], args[3]);
                        break;
                }
            }
            catch (Exception e)
            {
                Error(e.Message);
                return;
            }

            Exit();
        }
    }
}
