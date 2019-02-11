#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <fstream>

//include tesseract headers
#include <baseapi.h>
#include <allheaders.h>

using namespace std;

string tesseract_ocr(string preprocessed_file);
string tesseract_preprocess(string source_file);
void toClipboard(string s);


int main(int argc, char** argv)
{
	//check if file was passed as variable
	if (argc < 2){
		printf("Usage: %s source_image_filepath", argv[0]);
		return 1;
	}

	//set custom source file path here
	string source_file  = argv[1];

	//check if file exists
	std::ifstream infile(source_file);
	if (!infile.good()){
		printf("File not found");
		return 1;
	}

	//preprocess to convert to black white book-like text
	string preprocessed_file = tesseract_preprocess(source_file);

	//ocr and display results
	string ocr_result = tesseract_ocr(preprocessed_file);

	std::cout << ocr_result.c_str();

	toClipboard(ocr_result);
	
	return 1;
}

string tesseract_preprocess(string source_file){
	

	char tempPath[128];
	GetTempPathA(128, tempPath);
	strcat_s(tempPath, "preprocess_ocr.bmp");

	char preprocessed_file[MAX_PATH];
	strcpy_s(preprocessed_file, tempPath);


	BOOL perform_negate = TRUE;
	l_float32 dark_bg_threshold = 0.5f; // From 0.0 to 1.0, with 0 being all white and 1 being all black 

	int perform_scale = 1;
	l_float32 scale_factor = 3.5f;

	int perform_unsharp_mask = 1;
	l_int32 usm_halfwidth = 5;
	l_float32 usm_fract = 2.5f;

	int perform_otsu_binarize = 1;
	l_int32 otsu_sx = 2000;
	l_int32 otsu_sy = 2000;
	l_int32 otsu_smoothx = 0;
	l_int32 otsu_smoothy = 0;
	l_float32 otsu_scorefract = 0.0f;


	l_int32 status = 1;
	l_float32 border_avg = 0.0f;
	PIX *pixs = NULL;
	char *ext = NULL;


	// Read in source image 
	pixs = pixRead(source_file.c_str());


	// Convert to grayscale 
	pixs = pixConvertRGBToGray(pixs, 0.0f, 0.0f, 0.0f);


	if (perform_negate)
	{
		PIX *otsu_pixs = NULL;

		status = pixOtsuAdaptiveThreshold(pixs, otsu_sx, otsu_sy, otsu_smoothx, otsu_smoothy, otsu_scorefract, NULL, &otsu_pixs);


		// Get the average intensity of the border pixels,
		//with average of 0.0 being completely white and 1.0 being completely black. 
		border_avg = pixAverageOnLine(otsu_pixs, 0, 0, otsu_pixs->w - 1, 0, 1);                               // Top 
		border_avg += pixAverageOnLine(otsu_pixs, 0, otsu_pixs->h - 1, otsu_pixs->w - 1, otsu_pixs->h - 1, 1); // Bottom 
		border_avg += pixAverageOnLine(otsu_pixs, 0, 0, 0, otsu_pixs->h - 1, 1);                               // Left 
		border_avg += pixAverageOnLine(otsu_pixs, otsu_pixs->w - 1, 0, otsu_pixs->w - 1, otsu_pixs->h - 1, 1); // Right 
		border_avg /= 4.0f;

		pixDestroy(&otsu_pixs);

		// If background is dark 
		if (border_avg > dark_bg_threshold)
		{
			// Negate image 
			pixInvert(pixs, pixs);
	
		}
	}

	if (perform_scale)
	{
		// Scale the image (linear interpolation) 
		pixs = pixScaleGrayLI(pixs, scale_factor, scale_factor);
	}

	if (perform_unsharp_mask)
	{
		// Apply unsharp mask 
		pixs = pixUnsharpMaskingGray(pixs, usm_halfwidth, usm_fract);
	}

	if (perform_otsu_binarize)
	{
		// Binarize 
		status = pixOtsuAdaptiveThreshold(pixs, otsu_sx, otsu_sy, otsu_smoothx, otsu_smoothy, otsu_scorefract, NULL, &pixs);
	}

	
	// Write the image to file 
	status = pixWriteImpliedFormat(preprocessed_file, pixs, 0, 0);
	

	string out(preprocessed_file);

	return out;

}

string tesseract_ocr(string preprocessed_file)
{
	char *outText;
	Pix *image = pixRead(preprocessed_file.c_str());
	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

	
	TCHAR CurDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, CurDir);

	api->Init(CurDir, "eng");  
	api->SetPageSegMode(tesseract::PSM_AUTO_OSD); //PSM_SINGLE_BLOCK PSM_AUTO_OSD

	
	api->SetImage(image);

	outText = api->GetUTF8Text();
	
	system("cls");

	string out(outText);
	return out;
}

void toClipboard(const std::string s){
	OpenClipboard(0);
	EmptyClipboard();
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size());
	if (!hg){
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg), s.c_str(), s.size());
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
	GlobalFree(hg);
}
