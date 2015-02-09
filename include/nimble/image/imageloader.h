//
// Copyright (C) 2011 Vaugham Hong (vaughamhong@gmail.com)
//
// This file is subject to the terms and conditions defined in
// file 'license.txt', which is part of this source code package.
//

#ifndef __nimble_image_imageloader_h__
#define __nimble_image_imageloader_h__

//////////////////////////////////////////////////////////////////////////

#include <nimble/resource/resourceloader.h>

//////////////////////////////////////////////////////////////////////////

namespace nimble{
    namespace resource{
        class IResource;
    };
	namespace image{
        class IImage;
        
        //! Image file resource loader
        class FreeImageImageLoader
        : public resource::IResourceLoader{
        public:
            
            //! Constructor
            FreeImageImageLoader();
            //! Destructor
            virtual ~FreeImageImageLoader();
            
            //! loads an image
            //! \param path the path of the file we want to load
            virtual image::IImage* loadImage(const char *path);
            
            //! loads a resource
            //! \param path the path of the file we want to load
            virtual resource::IResource* loadResource(const char* path);
        };
	};
};

//////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////