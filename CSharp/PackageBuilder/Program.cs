using System.Collections.Generic;
using System.IO;
using System.Text;

namespace PackageBuilder
{
    public struct PakFile
    {
        public string Filename;
        public string ResourceKey;
        public long Size;
    }

    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length < 3)
            {
                return;
            }

            var gameDir = new DirectoryInfo(args[0]);
            var resourcesDir = new DirectoryInfo(args[0] + "/" + args[1]);
            var packageFilename = args[2];

            var files = Directory.GetFiles(resourcesDir.FullName, "*", SearchOption.AllDirectories);

            var asen = new UTF8Encoding();


            int headerSize = sizeof(int);
            var pakFiles = new List<PakFile>();

            foreach (var filename in files)
            {
                var pakFile = new PakFile
                {
                    ResourceKey = "./" + filename
                        .Substring(gameDir.FullName.Length)
                        .Replace('\\', '/'),
                    Filename = filename,
                    Size = (new FileInfo(filename)).Length
                };

                headerSize += (sizeof(int) + 2 * sizeof(long) + pakFile.ResourceKey.Length);

                pakFiles.Add(pakFile);
            }


            var fs = File.Create(packageFilename);
            using (var writer = new BinaryWriter(fs))
            {
                writer.Write(pakFiles.Count);

                long curPos = headerSize;
                foreach (var pakFile in pakFiles)
                {
                    var encoded = asen.GetBytes(pakFile.ResourceKey);

                    writer.Write(encoded.Length);
                    writer.Write(encoded);
                    writer.Write(curPos);
                    writer.Write(pakFile.Size);

                    curPos += pakFile.Size;
                }

                foreach (var pakFile in pakFiles)
                {
                    writer.Write(File.ReadAllBytes(pakFile.Filename));
                }
            }
            fs.Close();
        }
    }
}
