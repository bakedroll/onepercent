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
    class Bounds
    {
        private float _minX;
        private float _minY;
        private float _maxX;
        private float _maxY;

        public Bounds()
        {
            _minX = Single.MaxValue;
            _minY = Single.MaxValue;
            _maxX = Single.MinValue;
            _maxY = Single.MinValue;
        }

        public float CenterX
        {
            get { return _minX + Width / 2.0f; }
        }

        public float CenterY
        {
            get { return _minY + Height / 2.0f; }
        }

        public float Width
        {
            get { return _maxX - _minX; }
        }

        public float Height
        {
            get { return _maxY - _minY; }
        }

        public void Expand(float x, float y)
        {
            _minX = Math.Min(_minX, x);
            _minY = Math.Min(_minY, y);
            _maxX = Math.Max(_maxX, x);
            _maxY = Math.Max(_maxY, y);
        }
    }

    internal struct CountryInfo
    {
        public byte Id { get; set; }
        public string NameStr { get; set; }
        public byte[] Name { get; set; }
        public float Population { get; set; }
        public int Bip { get; set; }
        public Bounds Bounds { get; set; }

        public List<string> Neighbors { get; set; }
        public List<byte> NeighborsId { get; set; } 
    }

    class Program
    {
        private static int _pressKeyOnExit = 1;

        public static void ClearCurrentConsoleLine()
        {
            var currentLineCursor = Console.CursorTop;
            Console.SetCursorPosition(0, Console.CursorTop);
            Console.Write(new string(' ', Console.WindowWidth));
            Console.SetCursorPosition(0, currentLineCursor);
        }

        private static void Exit()
        {
            if (_pressKeyOnExit == 1)
            {
                Console.Write("Press any key to exit... ");
                Console.ReadKey();
            }
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
            Console.WriteLine("Reading image file {0}", filename);

            var inputBitmap = new Bitmap(filename);

            var width = inputBitmap.Width;
            var height = inputBitmap.Height;

            if ((width % cols != 0) && (height % rows != 0))
            {
                Console.WriteLine("Error: Wrong size");
                return;
            }

            var pwidth = width / cols;
            var pheight = height / rows;

            for (int y = 0; y < rows; y++)
            {
                for (int x = 0; x < cols; x++)
                {
                    var outputBitmap = new Bitmap(pwidth, pheight, inputBitmap.PixelFormat);

                    Console.WriteLine("Step {0}/{1}", y * cols + x + 1, rows * cols);

                    int percentCounter = 0;
                    Console.WriteLine("0%");
                    for (int py = 0; py < pheight; py++)
                    {
                        for (int px = 0; px < pwidth; px++)
                        {
                            outputBitmap.SetPixel(px, py, inputBitmap.GetPixel(x*pwidth + px, y*pheight + py));
                        }

                        Console.SetCursorPosition(0, Console.CursorTop - 1);
                        ClearCurrentConsoleLine();

                        percentCounter++;
                        Console.WriteLine("{0}%", percentCounter * 100 / pheight);
                    }

                    var outputFilename = string.Format("{0}{1}x{2}.png", outputdir, x, y);
                    Console.WriteLine("Saving {0}", outputFilename);
                    outputBitmap.Save(outputFilename, ImageFormat.Png);
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

        private static void ConvertCountriesMap(string tableFilename, string mapFilename, string greyscaleFilename, int newWidth, int newHeight, int newWidthMap, int newHeightMap, string binFilename)
        {
            Console.WriteLine("Reading map file {0}", mapFilename);

            var countriesBitmap = new Bitmap(mapFilename);

            Console.WriteLine("Reading table file {0}", tableFilename);

            var tableLines = File.ReadAllLines(tableFilename);

            Console.WriteLine("Writing bin file {0}", binFilename);

            var binFile = File.Create(binFilename);

            Console.WriteLine("Processing data...");

            var asen = new UTF8Encoding();

            var countries = new Dictionary<Color, CountryInfo>();

            int oldWidth = countriesBitmap.Width;
            int oldHeight = countriesBitmap.Height;

            byte counter = 0;
            foreach (var tLine in tableLines)
            {
                var values = tLine.Split(';').Select(x => x.Trim()).Where(x => !x.Equals(string.Empty)).ToArray();

                var color = ColorTranslator.FromHtml("#" + values[2]);

                if (countries.ContainsKey(color))
                {
                    Console.WriteLine("Warning: Color #{0} already exists ({1})", values[2], values[1]);
                }
                else
                {
                    countries.Add(color, new CountryInfo
                    {
                        Id = counter,
                        NameStr = values[1],
                        Name = asen.GetBytes(values[1]),
                        Population = float.Parse(values[3]),
                        Bip = int.Parse(values[4]),
                        Bounds = new Bounds(),
                        Neighbors = values.Reverse().Take(values.Length - 5).Reverse().ToList(),
                        NeighborsId = new List<byte>()
                    });

                    counter++;
                }
            }

            foreach (var country in countries)
            {
                foreach (var neighbor in country.Value.Neighbors)
                {
                    if (countries.Any(x => x.Value.NameStr.Equals(neighbor)))
                    {
                        var neighborCountry = countries.Single(x => x.Value.NameStr.Equals(neighbor));
                        country.Value.NeighborsId.Add(neighborCountry.Value.Id);
                    }
                    else
                    {
                        Console.WriteLine("Neighbor not found: {0} at: {1}", neighbor, country.Value.NameStr);
                    }
                }
            }

            int percentCounter = 0;
            Console.WriteLine("0%");

            var mapData = new byte[newWidth * newHeight];

            for (int y = 0; y < newHeight; y++)
            {
                for (int x = 0; x < newWidth; x++)
                {
                    var color = countriesBitmap.GetPixel((oldWidth * x) / newWidth, (oldHeight * y) / newHeight);

                    byte id;

                    if (countries.ContainsKey(color))
                    {
                        CountryInfo info;
                        countries.TryGetValue(color, out info);

                        float relX = (float)x / newWidth;
                        if (info.Id == 80 && relX < 0.2) // Russia
                        {
                            info.Bounds.Expand((float)x / (newWidth - 1) + 1.0f, (float)y / (newHeight - 1));
                        }
                        else
                        {
                            info.Bounds.Expand((float)x / (newWidth - 1), (float)y / (newHeight - 1));
                        }
                        

                        id = info.Id;
                    }
                    else
                    {
                        id = 255;
                    }

                    mapData[y * newWidth + x] = id;
                }

                Console.SetCursorPosition(0, Console.CursorTop - 1);
                ClearCurrentConsoleLine();

                percentCounter++;
                Console.WriteLine("{0}%", percentCounter * 100 / newHeight);
            }

            using (var writer = new BinaryWriter(binFile))
            {
                writer.Write(countries.Count);

                foreach (var country in countries)
                {
                    writer.Write(country.Value.Name.Length);
                    writer.Write(country.Value.Name);
                    writer.Write(country.Value.Population);
                    writer.Write(country.Value.Bip);
                    writer.Write(country.Value.Id);
                    writer.Write(country.Value.Bounds.CenterX);
                    writer.Write(country.Value.Bounds.CenterY);
                    writer.Write(country.Value.Bounds.Width);
                    writer.Write(country.Value.Bounds.Height);
                    writer.Write(country.Value.NeighborsId.Count);
                    country.Value.NeighborsId.ForEach(x => writer.Write(x));
                }

                writer.Write(newWidth);
                writer.Write(newHeight);
                writer.Write(mapData);
            }


            var resultBitmap = new Bitmap(newWidthMap, newHeightMap, PixelFormat.Format32bppArgb);

            Console.WriteLine("Processing country map...");

            percentCounter = 0;

            Console.WriteLine("0%");
            for (int y = 0; y < newHeightMap; y++)
            {
                for (int x = 0; x < newWidthMap; x++)
                {
                    var color = countriesBitmap.GetPixel((oldWidth * x) / newWidthMap, (oldHeight * y) / newHeightMap);

                    Color col;

                    if (countries.ContainsKey(color))
                    {
                        CountryInfo info;
                        countries.TryGetValue(color, out info);
                        col = Color.FromArgb(info.Id, info.Id, info.Id);
                    }
                    else
                    {
                        col = Color.White;
                    }

                    resultBitmap.SetPixel(x, y, col);
                }

                Console.SetCursorPosition(0, Console.CursorTop - 1);
                ClearCurrentConsoleLine();

                percentCounter++;
                Console.WriteLine("{0}%", percentCounter * 100 / newHeightMap);
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
                _pressKeyOnExit = Int32.Parse(args[1]);
                switch (command)
                {
                    case 0:
                        MergeImages(args[2], args[3], args[4], args[5], args[6]);
                        break;
                    case 1:
                        SubdivideImage(args[2], Int32.Parse(args[3]), Int32.Parse(args[4]), args[5]);
                        break;
                    case 2:
                        ConvertStarsMap(args[2], args[3]);
                        break;
                    case 3:
                        ConvertCountriesMap(args[2], args[3], args[4], Int32.Parse(args[5]), Int32.Parse(args[6]), Int32.Parse(args[7]), Int32.Parse(args[8]), args[9]);
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
