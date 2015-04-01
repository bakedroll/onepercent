using System;
using System.Drawing;
using System.Drawing.Imaging;

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
                    var outputBitmap = new Bitmap(pwidth, pheight, PixelFormat.Format24bppRgb);

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
