/* Copyright (c) 2006 Brandon Aaron (http://brandonaaron.net)
 * Dual licensed under the MIT (http://www.opensource.org/licenses/mit-license.php) 
 * and GPL (http://www.opensource.org/licenses/gpl-license.php) licenses.
 *
 * $LastChangedDate: 2007-06-20 03:23:36 +0200 (Mi, 20 Jun 2007) $
 * $Rev: 2110 $
 *
 * Version 2.1
 */

(function($){

/**
 * The bgiframe is chainable and applies the iframe hack to get 
 * around zIndex issues in IE6. It will only apply itself in IE 
 * and adds a class to the iframe called 'bgiframe'. The iframe
 * is appeneded as the first child of the matched element(s) 
 * with a tabIndex and zIndex of -1.
 * 
 * By default the plugin will take borders, sized with pixel units,
 * into account. If a different unit is used for the border's width,
 * then you will need to use the top and left settings as explained below.
 *
 * NOTICE: This plugin has been reported to cause perfromance problems
 * when used on elements that change properties (like width, height and
 * opacity) a lot in IE6. Most of these problems have been caused by 
 * the expressions used to calculate the elements width, height and 
 * borders. Some have reported it is due to the opacity filter. All 
 * these settings can be changed if needed as explained below.
 *
 * @example $('div').bgiframe();
 * @before <div><p>Paragraph</p></div>
 * @result <div><iframe class="bgiframe".../><p>Paragraph</p></div>
 *
 * @param Map settings Optional settings to configure the iframe.
 * @option String|Number top The iframe must be offset to the top
 * 		by the width of the top border. This should be a negative 
 *      number representing the border-top-width. If a number is 
 * 		is used here, pixels will be assumed. Otherwise, be sure
 *		to specify a unit. An expression could also be used. 
 * 		By default the value is "auto" which will use an expression 
 * 		to get the border-top-width if it is in pixels.
 * @option String|Number left The iframe must be offset to the left
 * 		by the width of the left border. This should be a negative 
 *      number representing the border-left-width. If a number is 
 * 		is used here, pixels will be assumed. Otherwise, be sure
 *		to specify a unit. An expression could also be used. 
 * 		By default the value is "auto" which will use an expression 
 * 		to get the border-left-width if it is in pixels.
 * @option String|Number width This is the width of the iframe. If
 *		a number is used here, pixels will be assume. Otherwise, be sure
 * 		to specify a unit. An experssion could also be used.
 *		By default the value is "auto" which will use an experssion
 * 		to get the offsetWidth.
 * @option String|Number height This is the height of the iframe. If
 *		a number is used here, pixels will be assume. Otherwise, be sure
 * 		to specify a unit. An experssion could also be used.
 *		By default the value is "auto" which will use an experssion
 * 		to get the offsetHeight.
 * @option Boolean opacity This is a boolean representing whether or not
 * 		to use opacity. If set to true, the opacity of 0 is applied. If
 *		set to false, the opacity filter is not applied. Default: true.
 * @option String src This setting is provided so that one could change 
 *		the src of the iframe to whatever they need.
 *		Default: "javascript:false;"
 *
 * @name bgiframe
 * @type jQuery
 * @cat Plugins/bgiframe
 * @author Brandon Aaron (brandon.aaron@gmail.com || http://brandonaaron.net)
 */
$.fn.bgIframe = $.fn.bgiframe = function(s) {
	// This is only for IE6
	if ( $.browser.msie && parseInt($.browser.version) <= 6 ) {
		s = $.extend({
			top     : 'auto', // auto == .currentStyle.borderTopWidth
			left    : 'auto', // auto == .currentStyle.borderLeftWidth
			width   : 'auto', // auto == offsetWidth
			height  : 'auto', // auto == offsetHeight
			opacity : true,
			src     : 'javascript:false;'
		}, s || {});
		var prop = function(n){return n&&n.constructor==Number?n+'px':n;},
		    html = '<iframe class="bgiframe"frameborder="0"tabindex="-1"src="'+s.src+'"'+
		               'style="display:block;position:absolute;z-index:-1;'+
			               (s.opacity !== false?'filter:Alpha(Opacity=\'0\');':'')+
					       'top:'+(s.top=='auto'?'expression(((parseInt(this.parentNode.currentStyle.borderTopWidth)||0)*-1)+\'px\')':prop(s.top))+';'+
					       'left:'+(s.left=='auto'?'expression(((parseInt(this.parentNode.currentStyle.borderLeftWidth)||0)*-1)+\'px\')':prop(s.left))+';'+
					       'width:'+(s.width=='auto'?'expression(this.parentNode.offsetWidth+\'px\')':prop(s.width))+';'+
					       'height:'+(s.height=='auto'?'expression(this.parentNode.offsetHeight+\'px\')':prop(s.height))+';'+
					'"/>';
		return this.each(function() {
			if ( $('> iframe.bgiframe', this).length == 0 )
				this.insertBefore( document.createElement(html), this.firstChild );
		});
	}
	return this;
};

// Add browser.version if it doesn't exist
if (!$.browser.version)
	$.browser.version = navigator.userAgent.toLowerCase().match(/.+(?:rv|it|ra|ie)[\/: ]([\d.]+)/)[1];

})(jQuery);

/* Copyright (c) 2007 Paul Bakaus (paul.bakaus@googlemail.com) and Brandon Aaron (brandon.aaron@gmail.com || http://brandonaaron.net)
 * Dual licensed under the MIT (http://www.opensource.org/licenses/mit-license.php)
 * and GPL (http://www.opensource.org/licenses/gpl-license.php) licenses.
 *
 * $LastChangedDate: 2007-06-22 04:38:37 +0200 (Fr, 22 Jun 2007) $
 * $Rev: 2141 $
 *
 * Version: 1.0b2
 */

(function($){

// store a copy of the core height and width methods
var height = $.fn.height,
    width  = $.fn.width;

$.fn.extend({
	/**
	 * If used on document, returns the document's height (innerHeight)
	 * If used on window, returns the viewport's (window) height
	 * See core docs on height() to see what happens when used on an element.
	 *
	 * @example $("#testdiv").height()
	 * @result 200
	 *
	 * @example $(document).height()
	 * @result 800
	 *
	 * @example $(window).height()
	 * @result 400
	 *
	 * @name height
	 * @type Object
	 * @cat Plugins/Dimensions
	 */
	height: function() {
		if ( this[0] == window )
			return self.innerHeight ||
				$.boxModel && document.documentElement.clientHeight || 
				document.body.clientHeight;
		
		if ( this[0] == document )
			return Math.max( document.body.scrollHeight, document.body.offsetHeight );
		
		return height.apply(this, arguments);
	},
	
	/**
	 * If used on document, returns the document's width (innerWidth)
	 * If used on window, returns the viewport's (window) width
	 * See core docs on height() to see what happens when used on an element.
	 *
	 * @example $("#testdiv").width()
	 * @result 200
	 *
	 * @example $(document).width()
	 * @result 800
	 *
	 * @example $(window).width()
	 * @result 400
	 *
	 * @name width
	 * @type Object
	 * @cat Plugins/Dimensions
	 */
	width: function() {
		if ( this[0] == window )
			return self.innerWidth ||
				$.boxModel && document.documentElement.clientWidth ||
				document.body.clientWidth;

		if ( this[0] == document )
			return Math.max( document.body.scrollWidth, document.body.offsetWidth );

		return width.apply(this, arguments);
	},
	
	/**
	 * Returns the inner height value (without border) for the first matched element.
	 * If used on document, returns the document's height (innerHeight)
	 * If used on window, returns the viewport's (window) height
	 *
	 * @example $("#testdiv").innerHeight()
	 * @result 800
	 *
	 * @name innerHeight
	 * @type Number
	 * @cat Plugins/Dimensions
	 */
	innerHeight: function() {
		return this[0] == window || this[0] == document ?
			this.height() :
			this.is(':visible') ?
				this[0].offsetHeight - num(this, 'borderTopWidth') - num(this, 'borderBottomWidth') :
				this.height() + num(this, 'paddingTop') + num(this, 'paddingBottom');
	},
	
	/**
	 * Returns the inner width value (without border) for the first matched element.
	 * If used on document, returns the document's Width (innerWidth)
	 * If used on window, returns the viewport's (window) width
	 *
	 * @example $("#testdiv").innerWidth()
	 * @result 1000
	 *
	 * @name innerWidth
	 * @type Number
	 * @cat Plugins/Dimensions
	 */
	innerWidth: function() {
		return this[0] == window || this[0] == document ?
			this.width() :
			this.is(':visible') ?
				this[0].offsetWidth - num(this, 'borderLeftWidth') - num(this, 'borderRightWidth') :
				this.width() + num(this, 'paddingLeft') + num(this, 'paddingRight');
	},
	
	/**
	 * Returns the outer height value (including border) for the first matched element.
	 * Cannot be used on document or window.
	 *
	 * @example $("#testdiv").outerHeight()
	 * @result 1000
	 *
	 * @name outerHeight
	 * @type Number
	 * @cat Plugins/Dimensions
	 */
	outerHeight: function() {
		return this[0] == window || this[0] == document ?
			this.height() :
			this.is(':visible') ?
				this[0].offsetHeight :
				this.height() + num(this,'borderTopWidth') + num(this, 'borderBottomWidth') + num(this, 'paddingTop') + num(this, 'paddingBottom');
	},
	
	/**
	 * Returns the outer width value (including border) for the first matched element.
	 * Cannot be used on document or window.
	 *
	 * @example $("#testdiv").outerHeight()
	 * @result 1000
	 *
	 * @name outerHeight
	 * @type Number
	 * @cat Plugins/Dimensions
	 */
	outerWidth: function() {
		return this[0] == window || this[0] == document ?
			this.width() :
			this.is(':visible') ?
				this[0].offsetWidth :
				this.width() + num(this, 'borderLeftWidth') + num(this, 'borderRightWidth') + num(this, 'paddingLeft') + num(this, 'paddingRight');
	},
	
	/**
	 * Returns how many pixels the user has scrolled to the right (scrollLeft).
	 * Works on containers with overflow: auto and window/document.
	 *
	 * @example $("#testdiv").scrollLeft()
	 * @result 100
	 *
	 * @name scrollLeft
	 * @type Number
	 * @cat Plugins/Dimensions
	 */
	/**
	 * Sets the scrollLeft property and continues the chain.
	 * Works on containers with overflow: auto and window/document.
	 *
	 * @example $("#testdiv").scrollLeft(10).scrollLeft()
	 * @result 10
	 *
	 * @name scrollLeft
	 * @param Number value A positive number representing the desired scrollLeft.
	 * @type jQuery
	 * @cat Plugins/Dimensions
	 */
	scrollLeft: function(val) {
		if ( val != undefined )
			// set the scroll left
			return this.each(function() {
				if (this == window || this == document)
					window.scrollTo( val, $(window).scrollTop() );
				else
					this.scrollLeft = val;
			});
		
		// return the scroll left offest in pixels
		if ( this[0] == window || this[0] == document )
			return self.pageXOffset ||
				$.boxModel && document.documentElement.scrollLeft ||
				document.body.scrollLeft;
				
		return this[0].scrollLeft;
	},
	
	/**
	 * Returns how many pixels the user has scrolled to the bottom (scrollTop).
	 * Works on containers with overflow: auto and window/document.
	 *
	 * @example $("#testdiv").scrollTop()
	 * @result 100
	 *
	 * @name scrollTop
	 * @type Number
	 * @cat Plugins/Dimensions
	 */
	/**
	 * Sets the scrollTop property and continues the chain.
	 * Works on containers with overflow: auto and window/document.
	 *
	 * @example $("#testdiv").scrollTop(10).scrollTop()
	 * @result 10
	 *
	 * @name scrollTop
	 * @param Number value A positive number representing the desired scrollTop.
	 * @type jQuery
	 * @cat Plugins/Dimensions
	 */
	scrollTop: function(val) {
		if ( val != undefined )
			// set the scroll top
			return this.each(function() {
				if (this == window || this == document)
					window.scrollTo( $(window).scrollLeft(), val );
				else
					this.scrollTop = val;
			});
		
		// return the scroll top offset in pixels
		if ( this[0] == window || this[0] == document )
			return self.pageYOffset ||
				$.boxModel && document.documentElement.scrollTop ||
				document.body.scrollTop;

		return this[0].scrollTop;
	},
	
	/** 
	 * Returns the top and left positioned offset in pixels.
	 * The positioned offset is the offset between a positioned
	 * parent and the element itself.
	 *
	 * @example $("#testdiv").position()
	 * @result { top: 100, left: 100 }
	 * 
	 * @name position
	 * @param Map options Optional settings to configure the way the offset is calculated.
	 * @option Boolean margin Should the margin of the element be included in the calculations? False by default.
	 * @option Boolean border Should the border of the element be included in the calculations? False by default.
	 * @option Boolean padding Should the padding of the element be included in the calculations? False by default.
	 * @param Object returnObject An object to store the return value in, so as not to break the chain. If passed in the
	 *                            chain will not be broken and the result will be assigned to this object.
	 * @type Object
	 * @cat Plugins/Dimensions
	 */
	position: function(options, returnObject) {
		var elem = this[0], parent = elem.parentNode, op = elem.offsetParent,
		    options = $.extend({ margin: false, border: false, padding: false, scroll: false }, options || {}),
			x = elem.offsetLeft,
			y = elem.offsetTop, 
			sl = elem.scrollLeft, 
			st = elem.scrollTop;
			
		// Mozilla and IE do not add the border
		if ($.browser.mozilla || $.browser.msie) {
			// add borders to offset
			x += num(elem, 'borderLeftWidth');
			y += num(elem, 'borderTopWidth');
		}

		if ($.browser.mozilla) {
			do {
				// Mozilla does not add the border for a parent that has overflow set to anything but visible
				if ($.browser.mozilla && parent != elem && $.css(parent, 'overflow') != 'visible') {
					x += num(parent, 'borderLeftWidth');
					y += num(parent, 'borderTopWidth');
				}

				if (parent == op) break; // break if we are already at the offestParent
			} while ((parent = parent.parentNode) && (parent.tagName.toLowerCase() != 'body' || parent.tagName.toLowerCase() != 'html'));
		}
		
		var returnValue = handleOffsetReturn(elem, options, x, y, sl, st);
		
		if (returnObject) { $.extend(returnObject, returnValue); return this; }
		else              { return returnValue; }
	},
	
	/**
	 * Returns the location of the element in pixels from the top left corner of the viewport.
	 *
	 * For accurate readings make sure to use pixel values for margins, borders and padding.
	 * 
	 * Known issues:
	 *  - Issue: A div positioned relative or static without any content before it and its parent will report an offsetTop of 0 in Safari
	 *    Workaround: Place content before the relative div ... and set height and width to 0 and overflow to hidden
	 *
	 * @example $("#testdiv").offset()
	 * @result { top: 100, left: 100, scrollTop: 10, scrollLeft: 10 }
	 *
	 * @example $("#testdiv").offset({ scroll: false })
	 * @result { top: 90, left: 90 }
	 *
	 * @example var offset = {}
	 * $("#testdiv").offset({ scroll: false }, offset)
	 * @result offset = { top: 90, left: 90 }
	 *
	 * @name offset
	 * @param Map options Optional settings to configure the way the offset is calculated.
	 * @option Boolean margin Should the margin of the element be included in the calculations? True by default.
	 * @option Boolean border Should the border of the element be included in the calculations? False by default.
	 * @option Boolean padding Should the padding of the element be included in the calculations? False by default.
	 * @option Boolean scroll Should the scroll offsets of the parent elements be included in the calculations? True by default.
	 *                        When true it adds the totla scroll offets of all parents to the total offset and also adds two properties
	 *                        to the returned object, scrollTop and scrollLeft. 
	 * @options Boolean lite Will use offsetLite instead of offset when set to true. False by default.
	 * @param Object returnObject An object to store the return value in, so as not to break the chain. If passed in the
	 *                            chain will not be broken and the result will be assigned to this object.
	 * @type Object
	 * @cat Plugins/Dimensions
	 */
	offset: function(options, returnObject) {
		var x = 0, y = 0, sl = 0, st = 0,
		    elem = this[0], parent = this[0], op, parPos, elemPos = $.css(elem, 'position'),
		    mo = $.browser.mozilla, ie = $.browser.msie, sf = $.browser.safari, oa = $.browser.opera,
		    absparent = false, relparent = false, 
		    options = $.extend({ margin: true, border: false, padding: false, scroll: true, lite: false }, options || {});
		
		// Use offsetLite if lite option is true
		if (options.lite) return this.offsetLite(options, returnObject);
		
		if (elem.tagName.toLowerCase() == 'body') {
			// Safari is the only one to get offsetLeft and offsetTop properties of the body "correct"
			// Except they all mess up when the body is positioned absolute or relative
			x = elem.offsetLeft;
			y = elem.offsetTop;
			// Mozilla ignores margin and subtracts border from body element
			if (mo) {
				x += num(elem, 'marginLeft') + (num(elem, 'borderLeftWidth')*2);
				y += num(elem, 'marginTop')  + (num(elem, 'borderTopWidth') *2);
			} else
			// Opera ignores margin
			if (oa) {
				x += num(elem, 'marginLeft');
				y += num(elem, 'marginTop');
			} else
			// IE does not add the border in Standards Mode
			if (ie && jQuery.boxModel) {
				x += num(elem, 'borderLeftWidth');
				y += num(elem, 'borderTopWidth');
			}
		} else {
			do {
				parPos = $.css(parent, 'position');
			
				x += parent.offsetLeft;
				y += parent.offsetTop;

				// Mozilla and IE do not add the border
				if (mo || ie) {
					// add borders to offset
					x += num(parent, 'borderLeftWidth');
					y += num(parent, 'borderTopWidth');

					// Mozilla does not include the border on body if an element isn't positioned absolute and is without an absolute parent
					if (mo && parPos == 'absolute') absparent = true;
					// IE does not include the border on the body if an element is position static and without an absolute or relative parent
					if (ie && parPos == 'relative') relparent = true;
				}

				op = parent.offsetParent;
				if (options.scroll || mo) {
					do {
						if (options.scroll) {
							// get scroll offsets
							sl += parent.scrollLeft;
							st += parent.scrollTop;
						}
				
						// Mozilla does not add the border for a parent that has overflow set to anything but visible
						if (mo && parent != elem && $.css(parent, 'overflow') != 'visible') {
							x += num(parent, 'borderLeftWidth');
							y += num(parent, 'borderTopWidth');
						}
				
						parent = parent.parentNode;
					} while (parent != op);
				}
				parent = op;

				if (parent.tagName.toLowerCase() == 'body' || parent.tagName.toLowerCase() == 'html') {
					// Safari and IE Standards Mode doesn't add the body margin for elments positioned with static or relative
					if ((sf || (ie && $.boxModel)) && elemPos != 'absolute' && elemPos != 'fixed') {
						x += num(parent, 'marginLeft');
						y += num(parent, 'marginTop');
					}
					// Mozilla does not include the border on body if an element isn't positioned absolute and is without an absolute parent
					// IE does not include the border on the body if an element is positioned static and without an absolute or relative parent
					if ( (mo && !absparent && elemPos != 'fixed') || 
					     (ie && elemPos == 'static' && !relparent) ) {
						x += num(parent, 'borderLeftWidth');
						y += num(parent, 'borderTopWidth');
					}
					break; // Exit the loop
				}
			} while (parent);
		}

		var returnValue = handleOffsetReturn(elem, options, x, y, sl, st);

		if (returnObject) { $.extend(returnObject, returnValue); return this; }
		else              { return returnValue; }
	},
	
	/**
	 * Returns the location of the element in pixels from the top left corner of the viewport.
	 * This method is much faster than offset but not as accurate. This method can be invoked
	 * by setting the lite option to true in the offset method.
	 *
	 * @name offsetLite
	 * @param Map options Optional settings to configure the way the offset is calculated.
	 * @option Boolean margin Should the margin of the element be included in the calculations? True by default.
	 * @option Boolean border Should the border of the element be included in the calculations? False by default.
	 * @option Boolean padding Should the padding of the element be included in the calculations? False by default.
	 * @option Boolean scroll Should the scroll offsets of the parent elements be included in the calculations? True by default.
	 *                        When true it adds the totla scroll offets of all parents to the total offset and also adds two properties
	 *                        to the returned object, scrollTop and scrollLeft. 
	 * @param Object returnObject An object to store the return value in, so as not to break the chain. If passed in the
	 *                            chain will not be broken and the result will be assigned to this object.
	 * @type Object
	 * @cat Plugins/Dimensions
	 */
	offsetLite: function(options, returnObject) {
		var x = 0, y = 0, sl = 0, st = 0, parent = this[0], op, 
		    options = $.extend({ margin: true, border: false, padding: false, scroll: true }, options || {});
				
		do {
			x += parent.offsetLeft;
			y += parent.offsetTop;

			op = parent.offsetParent;
			if (options.scroll) {
				// get scroll offsets
				do {
					sl += parent.scrollLeft;
					st += parent.scrollTop;
					parent = parent.parentNode;
				} while(parent != op);
			}
			parent = op;
		} while (parent && parent.tagName.toLowerCase() != 'body' && parent.tagName.toLowerCase() != 'html');

		var returnValue = handleOffsetReturn(this[0], options, x, y, sl, st);

		if (returnObject) { $.extend(returnObject, returnValue); return this; }
		else              { return returnValue; }
	}
});

/**
 * Handles converting a CSS Style into an Integer.
 * @private
 */
var num = function(el, prop) {
	return parseInt($.css(el.jquery?el[0]:el,prop))||0;
};

/**
 * Handles the return value of the offset and offsetLite methods.
 * @private
 */
var handleOffsetReturn = function(elem, options, x, y, sl, st) {
	if ( !options.margin ) {
		x -= num(elem, 'marginLeft');
		y -= num(elem, 'marginTop');
	}

	// Safari and Opera do not add the border for the element
	if ( options.border && ($.browser.safari || $.browser.opera) ) {
		x += num(elem, 'borderLeftWidth');
		y += num(elem, 'borderTopWidth');
	} else if ( !options.border && !($.browser.safari || $.browser.opera) ) {
		x -= num(elem, 'borderLeftWidth');
		y -= num(elem, 'borderTopWidth');
	}

	if ( options.padding ) {
		x += num(elem, 'paddingLeft');
		y += num(elem, 'paddingTop');
	}
	
	// do not include scroll offset on the element
	if ( options.scroll ) {
		sl -= elem.scrollLeft;
		st -= elem.scrollTop;
	}

	return options.scroll ? { top: y - st, left: x - sl, scrollTop:  st, scrollLeft: sl }
	                      : { top: y, left: x };
};

})(jQuery);

/*
 * jQuery delegate plug-in v1.0
 *
 * Copyright (c) 2007 Jörn Zaefferer
 *
 * $Id: jquery.delegate.js 4786 2008-02-19 20:02:34Z joern.zaefferer $
 *
 * Dual licensed under the MIT and GPL licenses:
 *   http://www.opensource.org/licenses/mit-license.php
 *   http://www.gnu.org/licenses/gpl.html
 */

// provides cross-browser focusin and focusout events
// IE has native support, in other browsers, use event caputuring (neither bubbles)

// provides delegate(type: String, delegate: Selector, handler: Callback) plugin for easier event delegation
// handler is only called when $(event.target).is(delegate), in the scope of the jQuery-object for event.target 

// provides triggerEvent(type: String, target: Element) to trigger delegated events
;(function($) {
	$.each({
		focus: 'focusin',
		blur: 'focusout'	
	}, function( original, fix ){
		$.event.special[fix] = {
			setup:function() {
				if ( $.browser.msie ) return false;
				this.addEventListener( original, $.event.special[fix].handler, true );
			},
			teardown:function() {
				if ( $.browser.msie ) return false;
				this.removeEventListener( original,
				$.event.special[fix].handler, true );
			},
			handler: function(e) {
				arguments[0] = $.event.fix(e);
				arguments[0].type = fix;
				return $.event.handle.apply(this, arguments);
			}
		};
	});

	$.extend($.fn, {
		delegate: function(type, delegate, handler) {
			return this.bind(type, function(event) {
				var target = $(event.target);
				if (target.is(delegate)) {
					return handler.apply(target, arguments);
				}
			});
		},
		triggerEvent: function(type, target) {
			return this.triggerHandler(type, [jQuery.event.fix({ type: type, target: target })]);
		}
	})
})(jQuery);

/*
 * jQuery Tooltip plugin 1.3
 *
 * http://bassistance.de/jquery-plugins/jquery-plugin-tooltip/
 * http://docs.jquery.com/Plugins/Tooltip
 *
 * Copyright (c) 2006 - 2008 Jörn Zaefferer
 *
 * $Id: jquery.tooltip.js 5741 2008-06-21 15:22:16Z joern.zaefferer $
 * 
 * Dual licensed under the MIT and GPL licenses:
 *   http://www.opensource.org/licenses/mit-license.php
 *   http://www.gnu.org/licenses/gpl.html
 */;(function($){var helper={},current,title,tID,IE=$.browser.msie&&/MSIE\s(5\.5|6\.)/.test(navigator.userAgent),track=false;$.tooltip={blocked:false,defaults:{delay:200,fade:false,showURL:true,extraClass:"",top:15,left:15,id:"tooltip"},block:function(){$.tooltip.blocked=!$.tooltip.blocked;}};$.fn.extend({tooltip:function(settings){settings=$.extend({},$.tooltip.defaults,settings);createHelper(settings);return this.each(function(){$.data(this,"tooltip",settings);this.tOpacity=helper.parent.css("opacity");this.tooltipText=this.title;$(this).removeAttr("title");this.alt="";}).mouseover(save).mouseout(hide).click(hide);},fixPNG:IE?function(){return this.each(function(){var image=$(this).css('backgroundImage');if(image.match(/^url\(["']?(.*\.png)["']?\)$/i)){image=RegExp.$1;$(this).css({'backgroundImage':'none','filter':"progid:DXImageTransform.Microsoft.AlphaImageLoader(enabled=true, sizingMethod=crop, src='"+image+"')"}).each(function(){var position=$(this).css('position');if(position!='absolute'&&position!='relative')$(this).css('position','relative');});}});}:function(){return this;},unfixPNG:IE?function(){return this.each(function(){$(this).css({'filter':'',backgroundImage:''});});}:function(){return this;},hideWhenEmpty:function(){return this.each(function(){$(this)[$(this).html()?"show":"hide"]();});},url:function(){return this.attr('href')||this.attr('src');}});function createHelper(settings){if(helper.parent)return;helper.parent=$('<div id="'+settings.id+'"><h3></h3><div class="body"></div><div class="url"></div></div>').appendTo(document.body).hide();if($.fn.bgiframe)helper.parent.bgiframe();helper.title=$('h3',helper.parent);helper.body=$('div.body',helper.parent);helper.url=$('div.url',helper.parent);}function settings(element){return $.data(element,"tooltip");}function handle(event){if(settings(this).delay)tID=setTimeout(show,settings(this).delay);else
show();track=!!settings(this).track;$(document.body).bind('mousemove',update);update(event);}function save(){if($.tooltip.blocked||this==current||(!this.tooltipText&&!settings(this).bodyHandler))return;current=this;title=this.tooltipText;if(settings(this).bodyHandler){helper.title.hide();var bodyContent=settings(this).bodyHandler.call(this);if(bodyContent.nodeType||bodyContent.jquery){helper.body.empty().append(bodyContent)}else{helper.body.html(bodyContent);}helper.body.show();}else if(settings(this).showBody){var parts=title.split(settings(this).showBody);helper.title.html(parts.shift()).show();helper.body.empty();for(var i=0,part;(part=parts[i]);i++){if(i>0)helper.body.append("<br/>");helper.body.append(part);}helper.body.hideWhenEmpty();}else{helper.title.html(title).show();helper.body.hide();}if(settings(this).showURL&&$(this).url())helper.url.html($(this).url().replace('http://','')).show();else
helper.url.hide();helper.parent.addClass(settings(this).extraClass);if(settings(this).fixPNG)helper.parent.fixPNG();handle.apply(this,arguments);}function show(){tID=null;if((!IE||!$.fn.bgiframe)&&settings(current).fade){if(helper.parent.is(":animated"))helper.parent.stop().show().fadeTo(settings(current).fade,current.tOpacity);else
helper.parent.is(':visible')?helper.parent.fadeTo(settings(current).fade,current.tOpacity):helper.parent.fadeIn(settings(current).fade);}else{helper.parent.show();}update();}function update(event){if($.tooltip.blocked)return;if(event&&event.target.tagName=="OPTION"){return;}if(!track&&helper.parent.is(":visible")){$(document.body).unbind('mousemove',update)}if(current==null){$(document.body).unbind('mousemove',update);return;}helper.parent.removeClass("viewport-right").removeClass("viewport-bottom");var left=helper.parent[0].offsetLeft;var top=helper.parent[0].offsetTop;if(event){left=event.pageX+settings(current).left;top=event.pageY+settings(current).top;var right='auto';if(settings(current).positionLeft){right=$(window).width()-left;left='auto';}helper.parent.css({left:left,right:right,top:top});}var v=viewport(),h=helper.parent[0];if(v.x+v.cx<h.offsetLeft+h.offsetWidth){left-=h.offsetWidth+20+settings(current).left;helper.parent.css({left:left+'px'}).addClass("viewport-right");}if(v.y+v.cy<h.offsetTop+h.offsetHeight){top-=h.offsetHeight+20+settings(current).top;helper.parent.css({top:top+'px'}).addClass("viewport-bottom");}}function viewport(){return{x:$(window).scrollLeft(),y:$(window).scrollTop(),cx:$(window).width(),cy:$(window).height()};}function hide(event){if($.tooltip.blocked)return;if(tID)clearTimeout(tID);current=null;var tsettings=settings(this);function complete(){helper.parent.removeClass(tsettings.extraClass).hide().css("opacity","");}if((!IE||!$.fn.bgiframe)&&tsettings.fade){if(helper.parent.is(':animated'))helper.parent.stop().fadeTo(tsettings.fade,0,complete);else
helper.parent.stop().fadeOut(tsettings.fade,complete);}else
complete();if(settings(this).fixPNG)helper.parent.unfixPNG();}})(jQuery);
