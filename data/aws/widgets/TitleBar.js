/** TitleBar factory. */
function TitleBar(inittext)
{
	var _widget = new Widget;
	var prefs = Skin.current;
	
	// Give it a pen
	_widget.SetPen(new Pen);
	
	// Initialize the text property.
	_widget.text = inittext;
		
	// Initialize the alignment
	_widget.hAlign = Pen.ALIGN_LEFT;
	_widget.vAlign = Pen.ALIGN_CENTER;
	
	//  Set initial size.  When you dock this bar, or stick it
	// in the top of a window as a child, it will resize. (Assuming
	// you use anchors if it's a child.)
	_widget.Resize(100, prefs.TitleBar.h);
	
	// Set the drawing function to be whatever the current style dictates.
	_widget.onDraw = prefs.Style.TitleBar;
	
	// If we get a mouse down, start the moving process.
	_widget.onMouseDown = function(buttons, widget_x, widget_y, screen_x, screen_y)
	{
		this.isMoving=true;	
		this.last_x=screen_x;
		this.last_y=screen_y;
		
		this.CaptureMouse();
	}
	
	// If the mouse is up, stop the moving process
	_widget.onMouseUp = function(buttons)
	{
		this.isMoving=false;
		
		this.ReleaseMouse();	
	}
	
	// If the mouse moves, we might be moving the widget
	_widget.onMouseMove = function(buttons, widget_x, widget_y, screen_x, screen_y)
	{
		if (this.isMoving)
		{
			var dx = screen_x-this.last_x,
				dy = screen_y-this.last_y;
				
			this.Move(dx, dy);
			
			this.last_x = screen_x;
			this.last_y = screen_y;				
		}			
	}
	
	// Create the buttons.
	_widget.Min = Button();
	_widget.Zoom = Button();
	_widget.Close = Button();
	
	// Add the children
	_widget.AddChild(_widget.Min);	
	_widget.AddChild(_widget.Zoom);
	_widget.AddChild(_widget.Close);
	
 	_widget.Min.ResizeTo(prefs.WindowMin.w, prefs.WindowMin.h);
 	_widget.Min.MoveTo(0, 5);
 	
 	_widget.Zoom.ResizeTo(prefs.WindowZoom.w, prefs.WindowZoom.h);
 	_widget.Zoom.MoveTo(0, 5);
 	
 	_widget.Close.ResizeTo(prefs.WindowClose.w, prefs.WindowClose.h);
 	_widget.Close.MoveTo(0, 5);
 	
 	_widget.Close.SetFrameAnchor(Widget.TRACK_EAST);
 	_widget.Close.SetMargin(3, Widget.MARGIN_EAST);
 	
 	_widget.Close.Dock(_widget.Zoom, Widget.DOCK_WEST);
 	_widget.Zoom.SetMargin(2, Widget.MARGIN_EAST);
 	
 	_widget.Zoom.Dock(_widget.Min, Widget.DOCK_WEST);	
 	_widget.Min.SetMargin(2, Widget.MARGIN_EAST);
 	
 	_widget.Min.onDrawContent = prefs.Style.WindowMin;
 	_widget.Zoom.onDrawContent = prefs.Style.WindowZoom; 	
 	_widget.Close.onDrawContent = prefs.Style.WindowClose;
	
	return _widget;
}
