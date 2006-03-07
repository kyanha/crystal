/** Button factory. */
function Button()
{
	var _widget = new Widget;
	var prefs = Skin.current;
	
	// Give it a pen
	_widget.SetPen(new Pen);
			
	// Setup the scroll bar
	_widget._state=false;
	
	// Invalidate and fire onChange when the value property is set.
	_widget.__defineSetter__("state", function(v) { this._state = v; this.Invalidate(); if (this.onChange) this.onChange(this); });	
	_widget.__defineGetter__("state", function() { return this._state; });	
	
	_widget.Resize(40, 20);
			
	// Set the drawing function to be whatever the current style dictates.
	_widget.onDraw = Skin.current.Style.Button;
	
	// Set the content drawing function.
	_widget.onDrawContent = null;
	
	// If we get a mouse down, change the button's appearance.
	_widget.onMouseDown = function(buttons, x, y)
	{
		this.state=true;		
	}
	
	// If the mouse is up, stop the moving process
	_widget.onMouseUp = function(buttons, x, y)
	{
		this.state=false;			
	}
	
	return _widget;
}
