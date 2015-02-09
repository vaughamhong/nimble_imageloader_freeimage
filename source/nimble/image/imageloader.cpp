//
// Copyright (C) 2011 Vaugham Hong (vaughamhong@gmail.com)
//
// This file is subject to the terms and conditions defined in
// file 'license.txt', which is part of this source code package.
//

#include <nimble/image/imageloader.h>
#include <nimble/image/image.h>
#include <nimble/image/color.h>
#include <nimble/image/colorconvert.h>
#include <nimble/core/logger.h>
#include <nimble/core/endian.h>
#include <nimble/resource/resource.h>
#include "freeImage.h"

//////////////////////////////////////////////////////////////////////////

using namespace nimble;
using namespace nimble::image;

//////////////////////////////////////////////////////////////////////////

static void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message){
    if(fif != FIF_UNKNOWN){
        core::logger_error("image", "Failed to load image - detected unknown format %s\n", FreeImage_GetFormatFromFIF(fif));
    }
}
//! returns the internal image format
image::eImageFormat freeimageFormatToInternalFormat(FREE_IMAGE_TYPE type, FREE_IMAGE_COLOR_TYPE colorFormat, unsigned bpp){
    if(type == FIT_RGBAF){
        return image::kImageFormatRGBF;
    }else if(type == FIT_BITMAP){
        if(bpp == 24){
            if(colorFormat == FIC_RGB){
                return image::kImageFormatR8G8B8;
            }
        }else if(bpp == 32){
            if(colorFormat == FIC_RGB){
                return image::kImageFormatR8G8B8A8;
            }else if(colorFormat == FIC_RGBALPHA){
                return image::kImageFormatR8G8B8A8;
            }
        }
    }
    return image::kImageFormatUnknown;
}

//////////////////////////////////////////////////////////////////////////

#define IMAGE_TUPLE(ENUM, PIXEL_SIZE) PIXEL_SIZE,
static uint32_t gBytesPerPixel[] ={
    IMAGE_TUPLESET
};
#undef IMAGE_TUPLE

//////////////////////////////////////////////////////////////////////////

//! Constructor
FreeImageImageLoader::FreeImageImageLoader(){
}
//! Destructor
FreeImageImageLoader::~FreeImageImageLoader(){
}
//! loads a resource
//! \param path the path of the file we want to load
resource::IResource* FreeImageImageLoader::load(const char* path){
    static bool freeimageInitialized = false;
    if(freeimageInitialized == false){
        FreeImage_Initialise();
        freeimageInitialized = true;
    }
    
    // load our (arbitrary) image
    FreeImage_SetOutputMessage(FreeImageErrorHandler);
    FREE_IMAGE_FORMAT srcFormat = FreeImage_GetFileType(path, 0);
    FIBITMAP *image = FreeImage_Load(srcFormat, path);
    FIBITMAP *b8g8r8a8Image = 0;
    
    // failed to load image
    if(image == 0){
        core::logger_warning("image", "Failed to load image - %s", path);
        return 0;
    }
    
    // get information on our image format
    FREE_IMAGE_TYPE type = FreeImage_GetImageType(image);
    FREE_IMAGE_COLOR_TYPE colorFormat = FreeImage_GetColorType(image);
    unsigned imageBpp = FreeImage_GetBPP(image);
    int imageWidth = FreeImage_GetWidth(image);
    int imageHeight = FreeImage_GetHeight(image);
    
    // note: There are two stages in getting a proper image to show up on screen.
    // 1. Nimble support - We load image data with a known format into our image buffer.
    // If the image format is unknown, we conver to R8G8B8A8 (32bit).
    // 2. Render device support - We bind our image to a rendering context. The render device
    // may not support the image format (even if we've successfully loaded it), in which case we may
    // get undetermined behaviour (relative to the render device implementation).
    
    // Optimizations - optimizing image handling will require a resource preprocessing step. This means having a pipeline
    // which converts our resources into target optimized / friendly formats. Each target will have
    // their own optimized formats, allowing us to slurp our data with no conversion and full render device support.
    
    // calculate our internal image format and get a pointer to our data
    image::eImageFormat imageFormat = freeimageFormatToInternalFormat(type, colorFormat, imageBpp);
    char *pData = 0;
    if(imageFormat != image::kImageFormatUnknown){
        // retreive our bits
        pData = (char*)FreeImage_GetBits(image);
    }else{
        // the image format is unknown, convert our image to R8G8B8A8
        imageFormat = image::kImageFormatR8G8B8A8;
        b8g8r8a8Image = FreeImage_ConvertTo32Bits(image);
        pData = (char*)FreeImage_GetBits(b8g8r8a8Image);
    }
    
    // Freeimage RGB vs BGR - When loading images using freeimage, little endian processors will have a pixel layout
    // in the form BGR and under big endian processors the pixel layout will be in the form RGB. Freeimage manages
    // this transparently so that it can load / save images natively on an OS platform.
    
    // GLES RGB - GLES only supports RGB formats, which means if freeimage ends up loading an image in BGRA format, we are
    // forced to do an extra conversion step. This is another instance where having a pipeline to preprocess
    // the image format into a native target format would allow us to remove an additional conversion step.
    if(core::isLittleEndian()){
        switch(imageFormat){
            case image::kImageFormatR8G8B8A8:{
                image::colorB8G8R8A8_t *pSrcData = (image::colorB8G8R8A8_t*)pData;
                image::colorR8G8B8A8_t *pDestData = (image::colorR8G8B8A8_t*)pData;
                image::convertFormat(pSrcData, pDestData, imageWidth, imageHeight);
                break;
            }
            case image::kImageFormatR8G8B8:{
                image::colorB8G8R8_t *pSrcData = (image::colorB8G8R8_t*)pData;
                image::colorR8G8B8_t *pDestData = (image::colorR8G8B8_t*)pData;
                image::convertFormat(pSrcData, pDestData, imageWidth, imageHeight);
                break;
            }
            default:
                core::logger_warning("image", "Internal format conversion from bgr to rgb failed");
                break;
        };
    }
    
    // create our image resource and copy image data
    resource::IResource *pResource = new /*( external dynamic )*/ resource::ResourceWrapper<image::Image>();
    image::IImage *pImage = dynamic_cast<image::IImage*>(pResource);
    pImage->initialize(imageWidth, imageHeight, imageFormat);
    pImage->copy(pData, imageWidth * imageHeight * gBytesPerPixel[imageFormat]);
    
    // clean up
    if(b8g8r8a8Image){
        FreeImage_Unload(b8g8r8a8Image);
    }
    if(image){
        FreeImage_Unload(image);
    }
    
    return pResource;
}

//////////////////////////////////////////////////////////////////////////