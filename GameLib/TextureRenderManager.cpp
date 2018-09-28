/***************************************************************************************
* Original Author:      Gabriele Giuseppini
* Created:              2018-09-28
* Copyright:            Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "TextureRenderManager.h"

#include "GameException.h"

void TextureRenderManager::UploadGroup(
    TextureGroup const & group,
    ProgressCallback const & progressCallback)
{
    float totalFramesCount = static_cast<float>(group.GetFrameCount());
    float currentFramesCount = 0;

    for (TextureFrameSpecification const & frameSpec : group.GetFrameSpecifications())
    {
        // Load frame
        TextureFrame frame = frameSpec.LoadFrame();

        // Notify progress
        currentFramesCount += 1.0f;
        progressCallback(currentFramesCount / totalFramesCount, "Loading textures...");

        // Create OpenGL handle
        GLuint openGLHandle;
        glGenTextures(1, &openGLHandle);

        // Bind texture
        glBindTexture(GL_TEXTURE_2D, openGLHandle);

        // Set repeat mode
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload texture data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame.Metadata.Size.Width, frame.Metadata.Size.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame.Data.get());
        if (GL_NO_ERROR != glGetError())
        {
            throw GameException("Error uploading texture onto GPU");
        }

        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);

        // Store data
        mFrameData.emplace(
            std::make_pair(
                frameSpec.Metadata.FrameId,
                FrameData(frameSpec.Metadata, openGLHandle)));
    }
}

void TextureRenderManager::UploadMipmappedGroup(
    TextureGroup const & group,
    ProgressCallback const & progressCallback)
{
    float totalFramesCount = static_cast<float>(group.GetFrameCount());
    float currentFramesCount = 0;

    for (TextureFrameSpecification const & frameSpec : group.GetFrameSpecifications())
    {
        // Load frame
        TextureFrame frame = frameSpec.LoadFrame();

        // Notify progress
        currentFramesCount += 1.0f;
        progressCallback(currentFramesCount / totalFramesCount, "Loading textures...");

        // Create OpenGL handle
        GLuint openGLHandle;
        glGenTextures(1, &openGLHandle);

        // Bind texture
        glBindTexture(GL_TEXTURE_2D, openGLHandle);

        // Upload texture
        GameOpenGL::UploadMipmappedTexture(std::move(frame));

        // Set repeat mode
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);

        // Store data
        mFrameData.emplace(
            std::make_pair(
                frameSpec.Metadata.FrameId,
                FrameData(frameSpec.Metadata, openGLHandle)));
    }
}