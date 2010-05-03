#include "vtkKWMyWindow.h"

//KWWidget headers:
#include "vtkKWApplication.h"
#include "vtkKWWindow.h"
#include "vtkKWRenderWidget.h"
#include "vtkKWSurfaceMaterialPropertyWidget.h"
#include "vtkKWSimpleAnimationWidget.h"
#include "vtkKWFrameWithScrollbar.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWScaleWithEntry.h"
#include "vtkKWWidgetsPaths.h"
#include "vtkKWChangeColorButton.h"
#include "vtkKWMenu.h"
#include "vtkKWLoadSaveDialog.h"
#include "vtkKWRadioButton.h"
#include "vtkKWRadioButtonSet.h"

//VTK headers:
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkStructuredPointsReader.h"
#include "vtkImageCast.h"
#include "vtkImageData.h"
#include "vtkCellPicker.h"
#include "vtkImagePlaneWidget.h"
#include "vtkLookupTable.h"
#include "vtkToolkits.h"
#include "vtkObjectFactory.h"

#include <vtksys/SystemTools.hxx>

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWMyWindow );
vtkCxxRevisionMacro( vtkKWMyWindow, "$Revision: 1.4 $");

//----------------------------------------------------------------------------
vtkKWMyWindow::vtkKWMyWindow()
{
  vtkReader  = vtkStructuredPointsReader::New();

  grayLookupTable = vtkLookupTable::New();
    grayLookupTable->SetHueRange(0, 0.0);
    grayLookupTable->SetSaturationRange(0, 0);
    grayLookupTable->SetValueRange(0 ,1);

  colorLookupTable = vtkLookupTable::New();
    colorLookupTable->SetHueRange(0.66667, 0.0);
    colorLookupTable->SetSaturationRange(1, 1);
    colorLookupTable->SetValueRange(1 ,1);
    colorLookupTable->SetAlphaRange(1, 1);
    colorLookupTable->SetNumberOfColors(256);
    colorLookupTable->Build();  
}

//----------------------------------------------------------------------------
vtkKWMyWindow::~vtkKWMyWindow()
{
  if (this->renderWidget)     this->renderWidget->Delete();
}

//----------------------------------------------------------------------------
void vtkKWMyWindow::CreateWidget()
{
  // Check if already created
  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }
  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();
  vtkKWApplication *app = this->GetApplication();

  //Prepare menus:
  int index;
  vtkKWMenu *openMenu = this->GetFileMenu() ;

  index = openMenu->InsertCommand(openMenu->GetNumberOfItems()-2,"Open VTK test data", this, "openVtkFileTestData");
    openMenu->SetBindingForItemAccelerator(index, openMenu->GetParentTopLevel());
    openMenu->SetItemHelpString(index, "Open VTK test data (SciRUN tooth file).");
  index = openMenu->InsertCommand(openMenu->GetNumberOfItems()-2,"Open &Vtk File", this, "openVtkFileDialog");
    openMenu->SetBindingForItemAccelerator(index, openMenu->GetParentTopLevel());
    openMenu->SetItemHelpString(index, "Open VTK Structured Points file format.");
  openMenu->InsertSeparator (openMenu->GetNumberOfItems()-2) ;

  // Add a render widget, attach it to the view frame, and pack
  renderWidget = vtkKWRenderWidget::New();
    renderWidget->SetParent(this->GetViewFrame());
    renderWidget->Create();
    renderWidget->SetRendererBackgroundColor(0.5, 0.6, 0.8);
    renderWidget->SetRendererGradientBackground(4);

    app->Script("pack %s -expand y -fill both -anchor c -expand y", renderWidget->GetWidgetName());

  // Create a scrolled frame
  vtkKWFrameWithScrollbar *vpw_frame = vtkKWFrameWithScrollbar::New();
    vpw_frame->SetParent(this->GetMainPanelFrame());
    vpw_frame->Create();

    app->Script("pack %s -side top -fill both -expand y", vpw_frame->GetWidgetName());

  vtkCellPicker* picker = vtkCellPicker::New();
    picker->SetTolerance(0.005);

  vtkProperty* ipwProp = vtkProperty::New();
  //assign default props to the ipw's texture plane actor

  vtkRenderWindowInteractor *sliceInteractor = renderWidget->GetRenderWindow()->GetInteractor();

  planeWidgetX = vtkImagePlaneWidget::New();
    planeWidgetX->SetInteractor( sliceInteractor );
    planeWidgetX->SetKeyPressActivationValue('x');
    planeWidgetX->SetPicker(picker);
    planeWidgetX->GetPlaneProperty()->SetColor(1,0,0);
    planeWidgetX->SetTexturePlaneProperty(ipwProp);
    planeWidgetX->TextureInterpolateOn();
    planeWidgetX->SetResliceInterpolateToCubic();
    planeWidgetX->DisplayTextOn();
    planeWidgetX->UpdatePlacement();

  planeWidgetY = vtkImagePlaneWidget::New();
    planeWidgetY->SetInteractor( sliceInteractor );
    planeWidgetY->SetKeyPressActivationValue('y');
    planeWidgetY->SetPicker(picker);
    planeWidgetY->GetPlaneProperty()->SetColor(1,1,0);
    planeWidgetY->SetTexturePlaneProperty(ipwProp);
    planeWidgetY->TextureInterpolateOn();
    planeWidgetY->SetResliceInterpolateToCubic();
    planeWidgetY->DisplayTextOn();
    planeWidgetY->UpdatePlacement();

  planeWidgetZ = vtkImagePlaneWidget::New();
    planeWidgetZ->SetInteractor( sliceInteractor );
    planeWidgetZ->SetKeyPressActivationValue('z');
    planeWidgetZ->SetPicker(picker);
    planeWidgetZ->GetPlaneProperty()->SetColor(0,0,1);
    planeWidgetZ->SetTexturePlaneProperty(ipwProp);
    planeWidgetZ->TextureInterpolateOn();
    planeWidgetZ->SetResliceInterpolateToCubic();
    planeWidgetZ->DisplayTextOn();
    planeWidgetZ->UpdatePlacement();

  double wl[2];
  planeWidgetZ->GetWindowLevel(wl);

  SetLutAsWhiteBlack();

  vtkKWFrameWithLabel *sliceposition_frame = vtkKWFrameWithLabel::New();
    sliceposition_frame->SetParent(vpw_frame->GetFrame());
    sliceposition_frame->Create();
    sliceposition_frame->SetLabelText("Change slice");

    app->Script("pack %s -side top -anchor nw -expand n -fill x -pady 2", sliceposition_frame->GetWidgetName());

  sliceX = vtkKWScaleWithEntry::New();
    sliceX->SetParent(sliceposition_frame->GetFrame());
    sliceX->Create();
    sliceX->SetLabelText("X");
    sliceX->SetValue(0);
    sliceX->SetCommand(this, "ChangeSliceX");

    app->Script("pack %s -side top -expand n -fill x -padx 2 -pady 2", sliceX->GetWidgetName());

  sliceY = vtkKWScaleWithEntry::New();
    sliceY->SetParent(sliceposition_frame->GetFrame());
    sliceY->Create();
    sliceY->SetLabelText("Y");
    sliceY->SetValue(0);
    sliceY->SetCommand(this, "ChangeSliceY");

    app->Script("pack %s -side top -expand n -fill x -padx 2 -pady 2", sliceY->GetWidgetName());

  sliceZ = vtkKWScaleWithEntry::New();
    sliceZ->SetParent(sliceposition_frame->GetFrame());
    sliceZ->Create();
    sliceZ->SetLabelText("Z");
    sliceZ->SetValue(0);
    sliceZ->SetCommand(this, "ChangeSliceZ");

    app->Script("pack %s -side top -expand n -fill x -padx 2 -pady 2", sliceZ->GetWidgetName());

  // Lut table
  vtkKWFrameWithLabel *lut_frame = vtkKWFrameWithLabel::New();
    lut_frame->SetParent(vpw_frame->GetFrame());
    lut_frame->Create();
    lut_frame->SetLabelText("Lut Table");

    app->Script("pack %s -side top -anchor nw -expand n -fill x -pady 2", lut_frame->GetWidgetName());

  vtkKWRadioButtonSet *lut_set = vtkKWRadioButtonSet::New();
    lut_set->SetParent(lut_frame->GetFrame());
    lut_set->Create();

    app->Script("pack %s -side top -anchor w", lut_set->GetWidgetName());

  vtkKWRadioButton *lut_wb = lut_set->AddWidget(0);
    lut_wb->SetText("White - Black");
    lut_wb->SetCommand(this, "SetLutAsWhiteBlack");

  vtkKWRadioButton *lut_bw = lut_set->AddWidget(1);
    lut_bw->SetText("Black - White");
    lut_bw->SetCommand(this, "SetLutAsBlackWhite");

  vtkKWRadioButton *lut_br = lut_set->AddWidget(2);
    lut_br->SetText("Blue - Red");
    lut_br->SetCommand(this, "SetLutAsBlueRed");

  vtkKWRadioButton *lut_rb = lut_set->AddWidget(3);
    lut_rb->SetText("Red - Blue");
    lut_rb->SetCommand(this, "SetLutAsRedBlue");


  
  // Create a simple animation widget
  vtkKWFrameWithLabel *animation_frame = vtkKWFrameWithLabel::New();
    animation_frame->SetParent(vpw_frame->GetFrame());
    animation_frame->Create();
    animation_frame->SetLabelText("Movie Creator");

    app->Script("pack %s -side top -anchor nw -expand n -fill x -pady 2", animation_frame->GetWidgetName());

  vtkKWSimpleAnimationWidget *animation_widget = vtkKWSimpleAnimationWidget::New();
    animation_widget->SetParent(animation_frame->GetFrame());
    animation_widget->Create();
    animation_widget->SetRenderWidget(renderWidget);
    animation_widget->SetAnimationTypeToCamera();

    app->Script("pack %s -side top -anchor nw -expand n -fill x", animation_widget->GetWidgetName());

  renderWidget->ResetCamera();
}

void vtkKWMyWindow::refreshApplicationAfterDataLoad()
{
  planeWidgetX->SetInput((vtkDataSet*)vtkReader->GetOutput());
  planeWidgetY->SetInput((vtkDataSet*)vtkReader->GetOutput());
  planeWidgetZ->SetInput((vtkDataSet*)vtkReader->GetOutput());
  planeWidgetX->SetPlaneOrientationToXAxes();
  planeWidgetY->SetPlaneOrientationToYAxes();
  planeWidgetZ->SetPlaneOrientationToZAxes();

  sliceX->SetRange(0,dimData[0]);
  sliceY->SetRange(0,dimData[1]);
  sliceZ->SetRange(0,dimData[2]);
  
  planeWidgetX->On();
  planeWidgetY->On();
  planeWidgetZ->On();
  
  renderWidget->Reset();
  renderWidget->Render();
}

void vtkKWMyWindow::openVtkFile(char *filename)
{
  vtkReader->SetFileName(filename);
  vtkReader->Update();
  
  rangeData = ((vtkImageData*)vtkReader->GetOutput())->GetScalarRange();
  dimData   = ((vtkImageData*)vtkReader->GetOutput())->GetDimensions();
  
  refreshApplicationAfterDataLoad();
}

void vtkKWMyWindow::openVtkFileTestData()
{
  openVtkFile("../test_data/tooth.vtk");
}

void vtkKWMyWindow::openVtkFileDialog()
{
  vtkKWLoadSaveDialog *open_dialog = vtkKWLoadSaveDialog::New();
  open_dialog->SetParent(this->GetParentTopLevel());
  open_dialog->RetrieveLastPathFromRegistry("OpenFilePath");
  open_dialog->Create();
  open_dialog->SetTitle("Open VTK file");
  int res = open_dialog->Invoke();
  
  if (res)
  {
    char *filename = open_dialog->GetFileName();
    openVtkFile(filename);
    open_dialog->SaveLastPathToRegistry("OpenFilePath");
  }
  open_dialog->Delete();
}

void vtkKWMyWindow::ChangeSliceX(int value)
{
  planeWidgetX->SetSliceIndex(value);
  renderWidget->Render();
}
void vtkKWMyWindow::ChangeSliceY(int value)
{
  planeWidgetY->SetSliceIndex(value);
  renderWidget->Render();
}
void vtkKWMyWindow::ChangeSliceZ(int value)
{
  planeWidgetZ->SetSliceIndex(value);
  renderWidget->Render();
}

void vtkKWMyWindow::SetLutAsWhiteBlack()
{
  grayLookupTable->SetValueRange(0 ,1);
  
  planeWidgetX->SetLookupTable(grayLookupTable);
  planeWidgetY->SetLookupTable(grayLookupTable);
  planeWidgetZ->SetLookupTable(grayLookupTable);
  renderWidget->Render();
}
void vtkKWMyWindow::SetLutAsBlackWhite()
{
  grayLookupTable->SetValueRange(1 ,0);
  
  planeWidgetX->SetLookupTable(grayLookupTable);
  planeWidgetY->SetLookupTable(grayLookupTable);
  planeWidgetZ->SetLookupTable(grayLookupTable);
  renderWidget->Render();
}
void vtkKWMyWindow::SetLutAsBlueRed()
{
  colorLookupTable->SetHueRange(0.66667, 0.0);
  
  planeWidgetX->SetLookupTable(colorLookupTable);
  planeWidgetY->SetLookupTable(colorLookupTable);
  planeWidgetZ->SetLookupTable(colorLookupTable);
  renderWidget->Render();
}
void vtkKWMyWindow::SetLutAsRedBlue()
{
  colorLookupTable->SetHueRange(0.0, 0.66667);
  
  planeWidgetX->SetLookupTable(colorLookupTable);
  planeWidgetY->SetLookupTable(colorLookupTable);
  planeWidgetZ->SetLookupTable(colorLookupTable);
  renderWidget->Render();
}
