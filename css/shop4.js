// Thousands separator, dot symbol in prices
var _shop_ts = '', _shop_dot = ',';

/*
 * Patches
 */
_patches = 0;
_patch_cart_custom_fields = 0x01;
// Other patches could be appended here on the next bits
// 0x02, 0x04, 0x08, 0x10 etc.
// Once patch run, _patches |= PATCH_NUMBER

/*
 * Cookies are enabled?
 */
if (navigator&&navigator.cookieEnabled == false){
	document.write(no_cookies_message);
}

// {{{ Utilities
/**
 * Trims a string
 * @author r.osmanov
 */
String.prototype.trim = function(s){
	var d = (s ? s : '[\\t\\s\\n\\r]');
	return this.replace(new RegExp("^"+d+"+"), '').replace(new RegExp(d+"+$"), '');
};

/**
 * Strip tags
 * @author r.osmanov
 */
String.prototype.stripTags = function(){
	return this.replace(/\<[^\>]+\>/g, '');
};

/**
 * Returns form element value
 */
function elVal(e){
	return (e.nodeName=='INPUT' ? e.value : e.nodeName=='SELECT' ? e.options[(e.selectedIndex >=0 ? e.selectedIndex : 0)].value : null);
}

/**
 * Returns form element node visible text
 *
 * @author r.osmanov
 * Defaults to innerHTML property
 * @param Node e
 * @return String
 */
function elText(e){
	if (e.nodeName=='SELECT' && e.selectedIndex>=0 && e.options[e.selectedIndex]!=undefined)
		return e.options[e.selectedIndex].innerHTML.stripTags();
	return (e.value != undefined ? e.value : (e.innerHTML ? e.innerHTML.stripTags() : ''));
}

/**
 * Set DOM node innerHTML
 */
function setHtml(e, html){
	if (typeof e == 'string') e = document.getElementById(e);
	if (e) e.innerHTML = html;
}

/**
 * Returns number as a rounded and formatted price
 */
function getPrice(n, fmt){
	return (fmt ? formatPrice(Math.round(n * 100) / 100)
		: (Math.round(n * 100) / 100));
}

/**
 * Format price
 * @author r.osmanov
 * @param Variant str
 */
function formatPrice(str) {//{{{
	if (typeof widgets != "undefined")
		return widgets.formatPrice(str, _shop_ts, _shop_dot);

	if (typeof str!='string') str = String(str);
	if (_shop_ts  == null) _shop_ts = ' ';
	if (_shop_dot== null) _shop_dot= '.';
	if (_shop_dot!= '.') str = str.replace('.', _shop_dot);

	var parts = str.split(_shop_dot), res = [], i;

	if (parts[0].length >= 4) {
		for (i = (parts[0].length - 1), j=1; i>=0; --i, ++j) {
			res.unshift(parts[0].charAt(i));
			if (j % 3 == 0 && i>0)
				res.unshift(_shop_ts);
		}

		return res.join('') + (parts[1] ? _shop_dot+ parts[1] : '');
	}
	return str;
}//}}}

//}}}

//{{{ Shop-specific functions
/**
 * Add list of products to the cart
 * Used with shop.shop_type = `price`
 */
function addListToCart(shop_id){//{{{
	if (false==isNaN(shop_id) && document.f.elements['product_ids[]'] && S3Shop) {

		var but = document.getElementById('submit_button');
		if (but)
			but.disabled = true;

		// Read cookies
		var total			= readCookie('CART_TOTAL_'+shop_id);
		var total_amount	= Number(readCookie('CART_TOTAL_AMOUNT_'+shop_id));
		var cart			= Infotex.cookie ? Infotex.cookie : '';
		var f				= document.f;
		var customFields	= S3Shop.customFieldNames || decodeURIComponent(readCookie('CART_CUSTOM_FIELDS_'+shop_id));

		// Amount limit stuff
		var has_prod_info = (window._shop != undefined && window._shop.products != undefined);
		var getAmountLimit = function(product_id) {
			return (has_prod_info && window._shop.products[product_id] != undefined
				? window._shop.products[product_id].amount : false);
		};

		// Parse custom fields to array
		if (typeof customFields == 'string')
			customFields		= customFields.split(';');

		// Get cart chunks as product_id=amount~param1Val~param2Val~...
		var cart_split		= cart.split(';');
		var cart_item_split, cart_hash = {}, found = false, product_id, price, amount, e, i, ids, ta, idx;

		for (i=0; i<cart_split.length; i++){
			// cart_item_split[0] = product_id
			// cart_item_split[1] = amount~param1Val~param2Val~...
			cart_item_split					= cart_split[i].split("=");

			if (cart_item_split.length != 2) continue;

			// cart_hash[product_id] = amount~param1Val~param2Val~...
			if (cart_hash[cart_item_split[0]] && typeof cart_hash[cart_item_split[0]].push=='function')
				cart_hash[cart_item_split[0]].push(cart_item_split[1]);
			else
				cart_hash[cart_item_split[0]] = [ cart_item_split[1] ];
		}

		cart	= "";
		ids		= f.elements['product_ids[]'];

		var delta, amount_in_cart;

		var getItemIndex = function(cart_hash_item_part, cfv){//{{{
			if (typeof cart_hash_item_part=="object"){
				for (var c=0, i=-1; c<cart_hash_item_part.length; ++c){
					i = cart_hash_item_part[c].indexOf('~');
					if (i!=-1){
						if (cart_hash_item_part[c].substr(i+1) == cfv)
							return c;
					}
				}
			}

			return false;
		};//}}}

		/**
		 * @brief Get amount of product entities in the cart
		 * @param Object cart_hash_item_part
		 * @return Integer
		 */
		var getItemAmount = function(cart_hash_item_part) {//{{{
			var r = 0, a;

			for (var i in cart_hash_item_part) {
				a = parseFloat(cart_hash_item_part[i]);
				if (!isNaN(a)) r += a;
			}

			return r;
		};//}}}

		/**
		 * @brief Fix exceeded amount limit.
		 * Normally won't be called at all. Last resort
		 * @param Object cart_hash_item_part
		 * @param Integer limit
		 * @return void
		 */
		var fixItemAmount = function(cart_hash_item_part, limit) {//{{{
			var a, amount_in_cart = getItemAmount(cart_hash_item_part);
			if (amount_in_cart && cart_hash_item_part.length) {
				var d, cf;

				for (var i = cart_hash_item_part.length - 1; i >= 0; i--) {
					a = parseFloat(cart_hash_item_part[i]);
					if (!isNaN(a)) {
						i = cart_hash_item_part[i].indexOf('~');
						/* Custom fields */
						cf = (i != -1 ? cart_hash_item_part[i].substr(i + 1) : '');

						/* Substract all but 1 */
						d		= Math.min(limit, a - 1);
						a		-= d;
						limit	-= d;

						cart_hash_item_part[i] = [a + '~' + cf];

						if (limit <= 0) break;
					}
				}
			};
		};//}}}

		/**
		 * @brief Process the next table row
		 * @author Ruslan Osmanov
		 * @param Variant i Index of the next row, or NULL/undefined for single row
		 * @return void
		 */
		var process = function(i) {//{{{
			var el = function(e, i) {
				return (i != undefined ? e[i] : e);
			};

			amount				= Number(elVal( el(f.elements['amounts[]'], i) ));
			if (!amount) return;
			product_id			= elVal( el(ids, i) );
			price				= Number(elVal( el(f.elements['prices[]'], i) ));
			customFieldValues	= getCustomFieldValues(f, customFields, i);
			amount_limit		= getAmountLimit(product_id);
			amount_in_cart		= getItemAmount(cart_hash[product_id]);

			if (amount_limit) {
				if (amount > amount_limit) {
					// Notify that amount limit exceeded
					widgets.msg(window._s3Lang['JS_SHOP_PRODUCT_AMOUNT_TOTAL']
						+ ': ' + amount_limit,
						el(document.f.elements['amounts[]'], i) );
					amount = amount_limit;
				}

				if (amount_in_cart > amount_limit) {
					/* Unlikely it falls here. But if so, the limit
					 * is somehow exceeded. Correct totals. */
					delta = amount_in_cart - amount;
					total -= price * delta;
					total_amount -= delta;
					fixItemAmount(cart_hash[product_id], amount_limit);
					return;
				}
			}

			if (amount_limit && amount_limit >= amount_in_cart) {
				delta = amount_limit - amount_in_cart;
				if (! isNaN(delta))
					amount = Math.min(amount, delta);
			}
			// Amount limit shouldn't be exceeded from here

			// Update totals
			total = Number(total) + price * amount;
			total_amount += amount;

			// Have product_id in hash?
			if (cart_hash[product_id] && typeof cart_hash[product_id].push=='function'){
				/* Possible index of entry with the same
				 product_id and custom field values */
				idx = getItemIndex(cart_hash[product_id], customFieldValues);

				// Have this product_id in the cart?
				if (false !== idx) {
					// Amount in cart
					ta	= $('input.product_ids[value=' + product_id + ']').length ? 0 : parseInt(cart_hash[product_id][idx]);

					// Update amount
					amount = (! isNaN(ta) ? ta : 0) + amount;

					// Modify the entry
					cart_hash[product_id][idx] = amount + '~' + customFieldValues;

				} else {
					/* There is no product_id with the exact same custom fields */
					cart_hash[product_id].push(amount + '~' + customFieldValues);
				}
			} else {
				// No such product_id in cart
				amount = (amount_limit ? Math.min(amount, amount_limit) : amount);
				cart_hash[product_id] = [ amount + '~' + customFieldValues];
			}
		};//}}}

		if (ids) {
			if (ids.nodeName != "INPUT")  {	// Multiple INPUTs
				for (i = 0; i < ids.length; i++) {
					if (ids[i]) process(i);
				}
			} else {	// Single INPUT
				process();
			}
		}

		// Gather cart_hash items in a string
		for (i in cart_hash){
			// Loop though
			for (var j=0; j<cart_hash[i].length; ++j){
				if (cart!="") cart += ";";
				cart += getCartChunk(i, cart_hash[i][j]);
			}
		}

		// Round total
		total = getPrice(total);

		// Write cookies
		Infotex.cookie = cart;
		Infotex.saveCookie();
		createCookie('CART_TOTAL_'+shop_id,			total,			10);
		createCookie('CART_TOTAL_AMOUNT_'+shop_id,	total_amount,	10);

		// Update DOM nodes
		setHtml('cart_total',		 formatPrice(total));
		setHtml('cart_total_amount', total_amount);

		// Reset product table form controls
		//resetOrderList();
	}

	return false;
}//}}}

/**
 * Returns cart cookie part for a product
 *
 * @author r.osmanov
 * @param id int product id
 * @param a int amount
 * @param String cfv Custom field values; string like param1Val~param2Val~...
 * @return String
 * product_id=amount~param1Val~param2Val~...
 */
function getCartChunk(id, a, cfv){
	return (arguments.length==2 ? (id + '=' + a) : (id + '=' + a + '~' + cfv));
}

/**
 * Returns custom field cart cookie string part like
 * param1Value~param2Value~...
 * @author r.osmanov
 * @param f Node Form
 * @param custom_fields Array Custom field names
 * @param postfix[optional] String form control name postfix
 * @return String
 */
function getCustomFieldValues(f, custom_fields, postfix){
	if (custom_fields && custom_fields.length){
		var c = '', e, i, j, s, postfix = (arguments.length==3 ? postfix : '');

		for(i=0; i<custom_fields.length; ++i){
			//e = f.elements[custom_fields[i] + postfix];
			// Some rows could be deleted
			for (; postfix < 100; ++postfix) {
				e = f.elements[custom_fields[i] + postfix];
				if (e) {
					break;
				};
			}
			S3Shop._last_postfix = postfix;
			// Append element text, or an empty string,
			// since the order of custom_fields should be kept
			//c += '~' + (e ? elText(e).trim() : '');

			c += '~';
			if (e) {
				s = elVal(e);
				c += (s ? s.trim() : '');
			}
		}

		return c.trim('~');
	}

	return '';
}

/**
 * Add a product to the cart
 *
 * @param Integer shop_id Shop ID
 * @param Integer product_id Product ID
 * @param Variant amount of product items to put into the cart
 * @param Node f
 * @note cookie format: product_id=amount~param1Val~param2Val~...
 */
function addToCart(shop_id, product_id, price, amount, f) {//{{{
	// Cast input
	product_id	= Number(product_id);
	amount		= Number(amount);
	price		= Number(price);

	var result = true, amount_in_cart = 0, amount_limit = amount;

	/* If window._shop.products.PRODUCT_ID.amount is set, then we must limit
	 * amount to that value */
	var has_amount_limit = (window._shop != undefined &&
		window._shop.products &&
		window._shop.products[product_id] != undefined ? true : false);
	if (has_amount_limit) {
		amount_limit = window._shop.products[product_id].amount;
		if (amount_limit < amount)
			amount = amount_limit;
	};

	// All is valid?
	if (S3Shop && false==isNaN(shop_id) && shop_id>0
		&& false==isNaN(product_id) && product_id>0
		&& false==isNaN(price)
		&& false==isNaN(amount) && amount>0) {

		var e, i, cfv, a, customFieldValues = '';

		// Read cookies
		var total			= readCookie('CART_TOTAL_'+shop_id);
		var cart			= Infotex.cookie ? Infotex.cookie : '';
		var total_amount	= 0;
		var customFields	= S3Shop.customFieldNames || decodeURIComponent(readCookie('CART_CUSTOM_FIELDS_'+shop_id));

		// Parse custom fields to array
		if (typeof customFields == 'string')
			customFields	= customFields.split(';');

		customFieldValues	= getCustomFieldValues(f, customFields);

		// Split cart string into `product_id=amount` chunks
		var cart_split		= cart.split(';'),
			cart_new		= "",
			found			= false, ta, cart_item_split;

		// Ensure total is of number data type
		if (isNaN(total)) total = Number(total);

		/*
		 * Check whether product is already in cart
		 * with the given set of custom field parameters(if any)
		 * @param product_id int ID of the product to be added to the cart
		 * @param item_id int Product ID of the product in the cart
		 * @param productCfv String Custom field values of the product to be added to the cart
		 * @param itemRhv String Cart product item right hand value like amount~param1Value~param2Value~...
		 */
		var isInCart = function (product_id, item_id, productCfv, itemRhv){
			// Has custom fields?
			if (productCfv != ''){
				// Cut off amount to get just a string with custom field values
				var ti	= cart_item_split[1].indexOf('~');
				var cfv = (ti != -1 ? cart_item_split[1].substr(ti + 1) : '');

				return (product_id == item_id && productCfv == cfv ? true : false);
			}

			// Compare just product IDs
			return (product_id == item_id ? true : false);
		};

		// Loop though cart chunks
		for (i = 0; i < cart_split.length; i++){
			// Split product item chunk
			cart_item_split = cart_split[i].split("=");

			// Valid chunk?
			if (cart_item_split.length == 2){
				// Parse amount
				a	= parseFloat(cart_item_split[1]);
				if (isNaN(a)) continue;

				if (product_id == parseInt(cart_item_split[0])) {
					amount_in_cart += a;
				};

				// Product not found & is aleady in the cart?
				if (!found
					&& isInCart(product_id, parseInt(cart_item_split[0]),
					customFieldValues, a)) {

					if (! (has_amount_limit && a >= amount_limit))
						total = Number(total) + price * amount;

					ta = amount + a;
					if (has_amount_limit && ta > amount_limit) {
						ta = amount_limit;
						result = false;
						if (widgets != undefined) {
							widgets.msg(window._s3Lang['JS_SHOP_PRODUCT_AMOUNT_TOTAL']
								+ ': ' + amount_limit, f);
						}
					}

					found = true;

					if (cart_new!="") cart_new += ';';
					cart_new	 += getCartChunk(product_id, ta, customFieldValues);

					// Update total amount
					total_amount += ta;
				} else { // This is a chunk with another product ID
					// Keep it in the cart without modifications
					if (cart_new!="") cart_new += ";";
					cart_new	 += getCartChunk(cart_item_split[0], cart_item_split[1]);

					// Update total amount
					total_amount += parseFloat(cart_item_split[1]);
				}
			}
		}
		/* amount_in_cart countains total amount of items with the same
		 * product_id (different combinations of custom fields) */

		// Product entry is new to the cart
		if (!found) {
			// Fix possible amount overflow
			if (has_amount_limit && (amount + amount_in_cart) > amount_limit ) {
				amount = amount_limit - amount_in_cart;
			};

			if (cart_new!="") cart_new += ";";
			cart_new	+= getCartChunk(product_id, amount, customFieldValues);
			total		= Number(total) + price * amount;

			// Update total amount
			total_amount += amount;
		}

		// Round total
		total = getPrice(total);

		// Write cookies
		Infotex.cookie = cart;
		Infotex.saveCookie();
		createCookie('CART_TOTAL_'+shop_id,			total,			10);
		createCookie('CART_TOTAL_AMOUNT_'+shop_id,	total_amount,	10);

		// Update DOM nodes
		setHtml('cart_total', formatPrice(total));
		setHtml('cart_total_amount', total_amount);

		// Success
		return result;
	}

	// Failure
	return false;
}//}}}

function productIsInCart(shop_id, product_id) {//{{{
	shop_id		= Number(shop_id);
	product_id	= Number(product_id);

	// Read cookies
	var cart = Infotex.cookie ? Infotex.cookie : '';
	return (new RegExp(product_id + '=\\d+', '')).test(cart);
}//}}}

/**
 * Return DOM node parent having provided tag name
 * @param obj Node Leverage node
 * @param tagName String Tag name of the parent
 */
function getParent(obj,tagName) {//{{{
	if (obj){
		var par = obj.parentNode;
		while (par&&(par.nodeName!=tagName))
			par = par.parentNode;

		return par;
	}
	return null;
}//}}}

function deleteRaw(o) {//{{{
	v = getParent(o,"TR");
	if (v)
		v.parentNode.removeChild(v);
}//}}}

// Dot
var dot = true;
var ttt = "2.23";
if (isNaN(ttt)) {
	dot = false;
}


function getEventTarget(e){
	if (!e) e = window.event;

	if (e.target) {
		if (e.target.nodeType == 3) e.target = e.target.parentNode;
		return e.target;

	}
	else if (e.srcElement)
		return e.srcElement;

}


function inputOnlyRealNumber(obj,e){

	var target = getEventTarget(e);

	if (target&&target.nodeName=="INPUT"&&target.type=="text"){

		var valueBefore = target.value;
		var value="";

		if (dot){

			value = valueBefore.replace(",",".");

		} else {

			value = valueBefore.replace(".",",");

		}

		value = value.replace(/[^\d\.,]+/,"");

		if (value.length>1)
			value = value.replace(/[0]*(\d*[\.,]?\d*).*/,"$1");

		if (value!=valueBefore){

			target.value =	value;

		}

		if (value != ""&&valueBefore==value) {

			return true;

		}

	}

	return false;

}

/**
 * Reassign cart items according to the cart table. Used on the cart page
 * @param Integer shop_id Shop ID
 * @return void
 */
function recountCart(shop_id) {//{{{
	/* The function could be called very frequently, each keypress! Thus, we
	 * wait for some user activity decrease */
	if (!window._shop) {
		window._shop = {};
	};
	if (window._shop.timerCart)
		window.clearTimeout(window._shop.timerCart);
	window._shop.timerCart = window.setTimeout((function() {
		// Fix cart custom fields
		patchCartCustomFields(shop_id);

		var customFields;
		var f = document.f;
		var total = 0, cart = "", product_id, price, amount, total_amount = 0, e, i,
			cfv, prevCfv, j, ids = f.elements['product_ids[]'];

		// Preliminary
		if (S3Shop == undefined) return false;

		// Amount limit stuff
		var has_prod_info = (window._shop != undefined && window._shop.products != undefined);
		var getAmountLimit = function(product_id) {
			return (has_prod_info && window._shop.products[product_id] != undefined
				? window._shop.products[product_id].amount : false);
		};
		var delta, items_in_cart = {};

		if (ids) {
			// Get custom field names
			customFields = S3Shop.customFieldNames || decodeURIComponent(readCookie('CART_CUSTOM_FIELDS_'+shop_id));
			// Parse custom fields
			if (typeof customFields == 'string')
				customFields = customFields.split(';');

			/* Returns element depending on whether j index provided */
			var el = function(e, j) {
				return (j != undefined ? e[j] : e);
			};

			/**
			 * @brief Returns total product amount already registered in
			 * combinations
			 * @author Ruslan Osmanov
			 * @param Integer product_id
			 * @return Integer
			 */
			var getItemAmount = function(product_id) {//{{{
				var r = 0;
				if (items_in_cart[product_id] != undefined) {
					for (var k in items_in_cart[product_id]) {
						r += items_in_cart[product_id][k];
					}
				};

				return r;
			};//}}}

			/**
			 * @brief Push amount onto the product accumulator
			 * @author Ruslan Osmanov
			 * @param Integer product_id
			 * @param Float a
			 * @return void
			 */
			var addItemAmount = function(product_id, a) {
				if (items_in_cart[product_id] == undefined)
					items_in_cart[product_id] = [];
				items_in_cart[product_id].push(a);
			};

			/**
			 * @brief Process the next table row
			 * @author Ruslan Osmanov
			 * @param Integer i
			 * @param Integer j
			 * @return void
			 */
			var process = function(i, j) {//{{{
				var iOld		= i;
				product_id		= elVal( el(ids, j) );
				price			= elVal( el(f.elements['prices[]'], j) );
				amount			= Number(elVal( el(f.elements['amounts[]'], j) ));
				cfv				= getCustomFieldValues(f, customFields, i);
				if (S3Shop._last_postfix) {
					if (i < S3Shop._last_postfix)
						i = S3Shop._last_postfix;
				}
				if (amount > 0) {
					amount_limit	= getAmountLimit(product_id);
					amount_in_cart	= getItemAmount(product_id);
				} else {
					amount_limit	= 0;
					amount_in_cart	= 0;
				};

				// Patch
				if (prevCfv && prevCfv == cfv)
					cfv = getCustomFieldValues(f, customFields, i);

				if (amount_limit) {
					if (amount > amount_limit) {
						// Notify that amount limit exceeded
						widgets.msg(window._s3Lang['JS_SHOP_PRODUCT_AMOUNT_TOTAL']
							+ ': ' + amount_limit,
							el(document.f.elements['amounts[]'], j) );
						amount = amount_limit;
						el(document.f.elements['amounts[]'], j).value = amount;

					};
					/* Omit possibility of amount_in_cart > amount_limit */
				};

				if (amount > 0) {
					if (amount_limit && amount_limit >= amount_in_cart) {
						delta = amount_limit - amount_in_cart;
						if (! isNaN(delta)) {
							amount = Math.min(amount, delta);
							el(document.f.elements['amounts[]'], j).value = amount;
						}
					}
					// Amount limit shouldn't be exceeded from here

					total += price * amount;
					total_amount += amount;

					addItemAmount(product_id, amount);

					if (cart != "") cart +=  ";";

					cart += getCartChunk(product_id, amount, cfv);
				}

				setHtml('res_' + product_id + i, getPrice(price * amount, true));

				return (i - iOld);
			};//}}}

			// Multiple INPUTs?
			if (ids.nodeName!="INPUT")	{
				// Loop though product ID fields(leverage)
				for (i=0,  j=0 ; j<=ids.length; i++, j++) {
					if (ids[j])
						i += process(i, j);
					prevCfv = cfv;
				}
			} else {	// Single INPUT
				// Patch.
				// Get product index
				// There could be a number of product rows in the cart table initially
				// So the product could have non-zero index
				var idx = 0;
				if (customFields && customFields[0]) {
					// Prevent from endless loop by limit of 50
					// (*rare* occasion when visitor removes 50 products from cart)
					for (e = undefined; e == undefined && idx < 50; ++idx) {
						e = f.elements[customFields[0] + idx];
					}
					if (e) --idx;
				}

				process(idx);
			}
		} else {					// No product IDs in the form
			// Clear cart DOM node contents
			setHtml('cart_div', '');
		}

		// Round total price
		total = getPrice(total);
		ft	  = formatPrice(total);

		// Cookies
		Infotex.cookie = cart;
		Infotex.saveCookie();
		createCookie('CART_TOTAL_'+shop_id,			total,			10);
		createCookie('CART_TOTAL_AMOUNT_'+shop_id,	total_amount,	10);

		// Update DOM nodes
		setHtml('total',				ft);
		setHtml('cart_total',			ft);
		setHtml('cart_total_amount',	total_amount);
	}), 500);
}//}}}

/**
 * Reset values in the order list
 */
function resetOrderList() {//{{{
	var but = document.getElementById('submit_button'), e;
	if (but) but.disabled = true;

	if (document.f.elements['amounts[]']) {
		if (document.f.elements['amounts[]'].nodeName!="INPUT")  {
			for (var i=0;i<document.f.elements['amounts[]'].length;i++){
				if (document.f.elements['amounts[]'][i])
					document.f.elements['amounts[]'][i].value = 0;
			}

		} else {
			document.f.elements['amounts[]'].value = 0;
		}
	}

	// Reset DOM node values just for the cart table
	setHtml('total', 0);
}//}}}

/**
 * Recount values in the order list
 * @obsolete
 */
function recountOrderList() {//{{{
	var total = 0, total_amount = 0, price, product_id, amount, e;

	if (document.f.elements['product_ids[]']) {
		if (document.f.elements['product_ids[]'].nodeName!="INPUT")  {						// Multiple INPUTs
			for (var i=0;i<document.f.elements['product_ids[]'].length;i++){

				if (document.f.elements['product_ids[]'][i]) {
					// Read product info
					product_id	= document.f.elements['product_ids[]'][i].value;
					price		= document.f.elements['prices[]'][i].value;
					amount		= Number(document.f.elements['amounts[]'][i].value);

					// Update total price
					total += price * amount;

					// Update total amount
					total_amount += amount;

					// Update result DOM node
					document.getElementById('res_'+product_id).innerHTML = formatPrice(Math.round((price*amount)*100)/100);
				}
			}
		} else {																			// Single INPUT
			// Read product info
			product_id	= document.f.elements['product_ids[]'].value;
			price		= document.f.elements['prices[]'].value;
			amount		= Number(document.f.elements['amounts[]'].value);

			// Update total price
			total		+= price * amount;

			// Update total amount
			total_amount += amount;

			// Update result DOM nodes
			setHtml('res_' + product_id, getPrice(price * amount, true));
		}
	}

	// Round total price
	if (isNaN(total)||total==0||total=='') total = 0;
	else total = getPrice(total);

	// Update DOM nodes
	setHtml('total', formatPrice(total));

	// Handle submit button availability
	var but = document.getElementById('submit_button');
	if (but) {
		if (total==0) but.disabled = true;
		else but.disabled = false;
	}
} // ## recountOrderList()//}}}

/**
 * Find position of the DOM node
 * @param Node obj DOM node which position is calculated
 */
function findPos(obj) {//{{{
	var result = {};
	result.x = 0;
	result.y = 0;

	if (obj.offsetParent) {
		while (obj.offsetParent) {
			result.y += obj.offsetTop;
			result.x += obj.offsetLeft;
			obj = obj.offsetParent;
		}

	} else {
		if (obj.x) result.x += obj.x;
		if (obj.y) result.y += obj.y;
	}

	return result;
} //}}}

function emptyInputBlur(obj,e) {//{{{
	var target = getEventTarget(e);

	if (target&&target.nodeName=="INPUT"&&target.type=="text"){
		if (target.value=="") target.value = 0;
		return true;
	}

	return false;
}//}}}

function showAddMessage(obj) {//{{{
	var pos = findPos(obj);
	var d = document.getElementById("shop-added");

	if (d) {
		d = d.cloneNode(true);
		d.style.display = 'block';
		d.style.left = (pos.x+10)+ 'px';
		d.style.top = (pos.y + obj.offsetHeight - d.offsetHeight) + 'px';
		document.body.appendChild(d);
		d.style.top = (parseInt(d.style.top)- d.offsetHeight-10) + 'px';


		window.setTimeout(function(){
			if (d&&d.parentNode)d.parentNode.removeChild(d); delete d;
		},500);
	}
}//}}}

/**
 * Add a list of products into the cart
 * @return boolean
 */
function addList(f,shop_id,func) {//{{{
	if (addListToCart(shop_id)){
		if (func) func(f);
		else showAddMessage(f);
	}
	return false;
} // ## addList()//}}}

/**
 * Add a product of the given amount to the cart
 * @param Integer shop_id Shop ID
 * @param Integer product_id Product ID
 * @param Float product_price Product price
 * @param Node f form node
 * @param Function func[optional] callback function
 */
function addProductForm(shop_id,product_id,product_price,f,func) {//{{{
	// Validate custom fields
	// Add product to cart
	if (addToCart(shop_id,product_id,product_price,f.product_amount.value, f)){
		// Callback
		if (func) func(f);
		else showAddMessage(f);
	}
	// Failed adding product to the cart
	f.product_amount.value = "1";

	return false;
} // ## addProductForm()//}}}

/**
 * Validate shop custum fields according to the fields `validator` properties
 * Show/hide error message nodes(if any)
 * @author r.osmanov
 * @param Node f Form
 * @return boolean
 * @note S3Shop.customFields global object with the custom fields required(assigned server-side)
 */
/*
 function validateCustomFields(f){//{{{
 var totalValid = true;

 // Preliminary
 if (S3Shop && "customFields" in S3Shop && f && f.nodeName=='FORM'){
 var a, e, c;
 for(a in S3Shop.customFields){
 c = S3Shop.customFields[a];

 // Try to get form element by name
 e = f.elements[a];
 if (e){
 // non-empty validator?
 if (�.validator==undefined || �.validator=='')
 continue;

 // Element value
 var v = elVal(e), isValid=true;

 // Validator type
 if (typeof c.validator=="string"){
 isValid = (new RegExp(c.validator)).test(v);
 }else if (typeof c.validator=="function"){
 isValid = c.validator(v);
 }

 // Show/hide error message
 if (e.error){
 e = document.getElementById(a + '_error_msg');
 if (e) e.style.display = (isValid ? 'none' : 'block');
 }

 // Total valid
 totalValid = (totalValid && isValid);
 }
 }
 }

 return totalValid;
 }//}}}
 */

/**
 * @brief Get custom field text by value
 * @author Ruslan Osmanov
 *
 * @param String name
 * @param mixed	val
 *
 * @return mixed
 */
function getCustomFieldTextByVal(name, val) {//{{{
	try {
		var field = S3Shop.customFields[name];
		switch (field.type) {
			case 'multiselect':
			case 'select':
				return field['options'][val];
				break;
			default:
				return val;
		}

	} catch (err) {

	}

	return val;
}//}}}

//}}}

function patchCartCustomFields(shopId) {//{{{
	if (_patches & _patch_cart_custom_fields) {
		return;
	};
	try {
		// Cart page?
		if ( (shopId > 0 || /(\?|\&)mode\=cart\1?/.test(location.search))
			&& S3Shop && S3Shop.customFields && S3Shop.customFieldNames.length) {
			if (!shopId) {
				// Try to read last shop ID from cookies
				shopId = readCookie('CART_LAST_SHOP_ID');
				if (shopId == null) {
					throw "No last shop ID cookie";
				};
			};

			var i = 0, j = 0, f = document.forms.f, v, re, name = '',
				customFieldNames = [], cart = '', cartHash = {};
			var ids = f.elements['product_ids[]'];
			if (!ids) {
				return;
			};

			// Get custom field names
			customFieldNames = S3Shop.customFieldNames ||
				decodeURIComponent(readCookie('CART_CUSTOM_FIELDS_' + shopId));
			if (typeof customFieldNames == 'string') {
				customFieldNames = customFieldNames.split(';');
			};

			// Get cart chunks as product_id=amount~param1Val~param2Val~...
			cart = Infotex.cookie ? Infotex.cookie.split(';') : [''];
			for (i = 0; i < cart.length; i++) {
				v = cart[i].split('=');
				if (v.length == 2 && v[0] > 0) {
					if (cartHash[v[0]] == undefined) {
						cartHash[v[0]] = [];
					}
					cartHash[v[0]].push(v[1].split('~'));
				};
			}
			/*
			 * cartHash = {
			 *	productId : [
			 *		[amount, value1, value2, ...],
			 *		...
			 *	],
			 *	...
			 * }
			 */

			// Loop over product INPUTs, and reassign custom field INPUT values
			if (ids.nodeName != 'INPUT') { // multiple inputs
				for (i = 0; i < ids.length; i++) {
					productId	= elVal(f.elements['product_ids[]'][i]);

					for (j = 0; j < customFieldNames.length; j++) {
						name = customFieldNames[j] + i;
						if (f.elements[name] == undefined
							|| cartHash[productId][0] == undefined
							|| cartHash[productId][0][j + 1] == undefined) {
							continue;
						};

						if (getCustomFieldTextByVal(customFieldNames[j], cartHash[productId][0][j + 1]) != f.elements[name].value) {
							// TODO: iterate other members of cartHash[productId]
							continue;
						};
						f.elements[name].value = cartHash[productId][0][j + 1];
					}

					// Delete the first bunch of productId custom fields
					cartHash[productId].shift();
				}
			} else {						// single input
				productId	= elVal(f.elements['product_ids[]']);

				for (j = 0, i = 0; j < customFieldNames.length; j++) {
					name = customFieldNames[j] + i;
					if (f.elements[name] == undefined
						|| cartHash[productId][0] == undefined
						|| cartHash[productId][0][j + 1] == undefined) {
						continue;
					};

					if (getCustomFieldTextByVal(customFieldNames[j], cartHash[productId][0][j + 1]) != f.elements[name].value) {
						continue;
					};
					f.elements[name].value = cartHash[productId][0][j + 1];
				}
			};

			_patches |= _patch_cart_custom_fields;
		};
	} catch (err) {
		//console.log(err);
	}

}//}}}

/*
 * Add window.onload event
 */
widgets.addOnloadEvent(function() {

	/* {{{ Patch: replace cart form custom field values with those from cookies,
	 * since prevliously we kept SELECT OPTION texts instead of values
	 * Now CART_{$shop_id} cookie will store element values only
	 * @author Ruslan Osmanov
	 */
	patchCartCustomFields(null);
	/* }}} */

});