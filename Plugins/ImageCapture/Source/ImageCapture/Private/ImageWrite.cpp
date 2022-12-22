#include "ImageWrite.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"

FImageWriterTask::FImageWriterTask(TArray<FColor> Image, int Width, int Height, FString FileName)
    : Image(Image), FileName(FileName), ImageWidth(Width), ImageHeight(Height)
    {}

/**
 * @brief Saves an image array to disk
 */
void FImageWriterTask::DoWork(){
    // ready and rendered
    static IImageWrapperModule& ImageWrapperModule =
        FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    static TSharedPtr<IImageWrapper> ImageWrapper =
        ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
    ImageWrapper->SetRaw(
        Image.GetData(),
        Image.GetAllocatedSize(),
        ImageWidth,
        ImageHeight,
        ERGBFormat::BGRA,
        8);
    const TArray64<uint8>& compressedImage = ImageWrapper->GetCompressed(0);

    FFileHelper::SaveArrayToFile(compressedImage, *FileName);
    UE_LOG(LogTemp, Verbose, TEXT("Written image: %s to disk"), *FileName);
}
