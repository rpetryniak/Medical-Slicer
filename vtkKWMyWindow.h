#ifndef __vtkKWMyWindow_h
#define __vtkKWMyWindow_h

#include "vtkKWWindow.h"

class vtkKWRenderWidget;
class vtkKWScaleWithEntry;
class vtkImagePlaneWidget;
class vtkStructuredPointsReader;
class vtkLookupTable;

class vtkKWMyWindow : public vtkKWWindow
{
public:
  static vtkKWMyWindow* New();
  vtkTypeRevisionMacro(vtkKWMyWindow,vtkKWWindow);

  // Callbacks
  virtual void ChangeSliceX(int value);
  virtual void ChangeSliceY(int value);
  virtual void ChangeSliceZ(int value);
  virtual void SetLutAsWhiteBlack();
  virtual void SetLutAsBlackWhite();
  virtual void SetLutAsBlueRed();
  virtual void SetLutAsRedBlue();
  
  virtual void openVtkFileDialog();
  void openVtkFile(char *filename);
  void openVtkFileTestData();
  
protected:
  vtkKWMyWindow();
  ~vtkKWMyWindow();
  
  virtual void CreateWidget();

  vtkKWRenderWidget           *renderWidget;
  vtkKWScaleWithEntry         *sliceX;
  vtkKWScaleWithEntry         *sliceY;
  vtkKWScaleWithEntry         *sliceZ;
  vtkImagePlaneWidget         *planeWidgetX;
  vtkImagePlaneWidget         *planeWidgetY;
  vtkImagePlaneWidget         *planeWidgetZ;
  vtkStructuredPointsReader   *vtkReader;

  vtkLookupTable              *grayLookupTable;
  vtkLookupTable              *colorLookupTable;
  
  int     isoLevel;
  float   isoOpacity;
  double  *rangeData;
  int     *dimData;
private:
  vtkKWMyWindow(const vtkKWMyWindow&);   // Not implemented.
  void operator=(const vtkKWMyWindow&);  // Not implemented.

  void refreshApplicationAfterDataLoad();
};

#endif
