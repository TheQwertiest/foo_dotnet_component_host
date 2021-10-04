using System;
using System.Drawing;

namespace Qwr.ComponentInterface
{
    public enum ArtId
    {
        CoverFront = 0,
        CoverBack = 1,
        Disc = 2,
        Icon = 3,
        Artist = 4
    }

    public interface IMetadbHandle : IDisposable
    {
        Bitmap Artwork(ArtId artId);
        Bitmap ArtworkStub(ArtId artId);
        double Length();
        string Path();
        IFileInfo FileInfo();
    }
}