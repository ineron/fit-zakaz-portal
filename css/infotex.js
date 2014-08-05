/*
 * @file infotex.js
 * @description infotex
 * @date 2011.12.20 10:00
 * @author swdalex (Alexey Chavkin) <swdalex@gmail.com>
 * @copyright Copyright (c) 2011, MegaGroup http://megagroup.ru
 * @version 1.7 (2012.07.17)
 */

var Infotex = {
	ver_id: 0,
	shop_id: 0,
	images_dir: 'http://' + window.location.hostname + '/images/',
	_zIndex: 99,
	prevent_hash: false,
	cookie: false,
	ajax: {
		xhr: false,
		t: 0
	},
	modalOn: function() {
		Infotex._zIndex++;
		$('#black').css('z-index', Infotex._zIndex).show();
	},
	modalOff: function() {
		$('#black').off('click');
		if (!$('#popup').length) {
			$('#black').hide();
		}
	},
	loaderOn: function() {
		$('#black').css('z-index', '+=3');
		$('#loader').show();
	},
	loaderOff: function() {
		$('#black').css('z-index', '-=3');
		$('#loader').hide();
	},
	setHTML: function(data) {
		$('body').html(data).attr('id', $('#black').data('parent'));
	},
	load: function(url) {
		window.location = url;
		/*
		 Infotex.modalOn();
		 Infotex.loaderOn();

		 if (~url.indexOf('?')) {
		 url += '&jx=1';
		 } else {
		 url += '?jx=1';
		 }

		 $.ajax({
		 url: url,
		 method: 'get',
		 success: function(data) {
		 Infotex.setHTML(data);

		 window.setTimeout(function() {
		 Infotex.modalOff();
		 Infotex.modalOff();

		 Infotex.onLoad();
		 }, 500);
		 }
		 });
		 */
	},
	bind_ajax: function() {
		$('form').ajaxForm({
			data: {'jx': 1},
			beforeSubmit: function() {
				Infotex.modalOn();
				Infotex.loaderOn();
			},
			success: function(data, status, xhr, form) {
				Infotex.setHTML(data);

				window.setTimeout(function() {
					Infotex.modalOff();
					Infotex.modalOff();

					Infotex.prevent_hash = true;
					window.location.hash = form.attr('action');
					Infotex.onLoad();
				}, 500);
			}
		});

		$('a').on('click', function() {
			window.location.hash = $(this).attr('href');
			return false;
		});
	},
	onLoad: function() {
		 $.ajaxSetup({
			timeout: 10000,
			error: function() {
				alert('Произошла неизвестная ошибка. Обратитесь к администратору сайта.');
				Infotex.loaderOff();
				Infotex.modalOff();
			}
		});

		$('a').each(function() {
			if (!!$(this).attr('href') && $(this).attr('href').length > 1 && !$(this).hasClass('highslide')) {
				$(this).click(function(e) {
					e.preventDefault();
					var url = $(this).attr('href');
					Infotex.saveCookie(true, function() {
						window.location = url;
					});
					return false;
				});
			}
		});

		h_u();

		if ($('#catalog_f').length) {
			var inputs = $('#catalog_f input[name="amounts[]"]');
			$('#catalog_f')
				.on('keyup', function(event) {
					if(inputOnlyRealNumber(this, event)) {
						recountOrderList();
					}
				})
				.on('submit', function(event) {
					return addList(this, Infotex.shop_id);
				})
				.find('td.cell_7 span.amount a.dec')
				.on('click', function() {
					if (+$(this).next().val() <= 1) {
						$(this).next().val(0);
					} else {
						$(this).next().val(+$(this).next().val()-1);
					}
					$(this).next().change();
				}).next()
				.on('change', function() {
					if (+$(this).val() <= 0) {
						$(this).removeClass('blue');
					} else {
						$(this).addClass('blue');
					}
					addList(this, Infotex.shop_id);
				}).
				on('focus', function() {
					this.select();
				})
				.on('keyup', function(event) {
					if (event.keyCode == '13') {
						event.preventDefault();
						var next = inputs.eq( inputs.index($(this)) + 1 );
						if (next.length) {
							next.focus();
						}
					}
				})
				.tooltip({
					bodyHandler: function() {
						return $(this).val();
					}
				})
				.next()
				.on('click', function() {
					$(this).prev().val(+$(this).prev().val()+1); $(this).prev().change();
				});

			$('#catalog_f td.cell_3, #catalog_f td.cell_4, #catalog_f td.cell_5').tooltip();

			if ($('#search_result').length) {
				$(window).scrollTop($('#search_result').offset().top - 50);
				$('#search_result').parents('tr:first').find('input[name="amounts[]"]').focus().select();
			}

			if ($('#img_off').length) {
				$('#img_off').click(function() {
					if (readCookie('IMAGES_OFF_' + Infotex.shop_id)) {
						createCookie('IMAGES_OFF_' + Infotex.shop_id, '0', 10);
					} else {
						createCookie('IMAGES_OFF_' + Infotex.shop_id, '1', 10);
					}
					window.location.reload();
					return false;
				});
			}
		}

		if ($('#cart_f').length) {
			var inputs = $('#cart_f input[name="amounts[]"]');
			$('#cart_f')
				.on('keyup', function(event) {
					if(inputOnlyRealNumber(this, event)) {
						recountCart(Infotex.shop_id);
					}
				})
				.on('submit', function(event) {
					return false;
				})
				.find('span.amount a.dec')
				.on('click', function() {
					if (+$(this).next().val() <= 1) {
						$(this).next().val(0);
					} else {
						$(this).next().val(+$(this).next().val()-1);
					}
					$(this).next().change();
				}).next()
				.on('change', function() {
					if (+$(this).val() <= 0) {
						$(this).removeClass('blue');
					} else {
						$(this).addClass('blue');
					}
					recountCart(Infotex.shop_id);
				})
				.on('focus', function() {
					this.select();
				})
				.on('keyup', function(event) {
					if (event.keyCode == '13') {
						event.preventDefault();
						var next = inputs.eq( inputs.index($(this)) + 1 );
						if (next.length) {
							next.focus();
						}
					}
				})
				.tooltip({
					bodyHandler: function() {
						return $(this).val();
					}
				})
				.next()
				.on('click', function() {
					$(this).prev().val(+$(this).prev().val()+1); $(this).prev().change();
				});
		}

		if ($('#update_info a.messages').length) {
			$('#update_info a.messages').on('click', function() {
				Infotex.modalOn();
				Infotex.loaderOn();

				var href = $(this).attr('href');

				$.ajax({
					url: '/my/reload_messages.cphp',
					method: 'get',
					dataType: 'json',
					success: function(data) {
						window.location = href;
					}
				});

				return false;
			});
		}

		if ($('#message_form').length) {
			Infotex.bindMessageForm();
		}

		if ($('#order_import_form').length) {
			Infotex.bindOrderImportForm();
		}

		//Infotex.bind_ajax();
	},
	bindMessageForm: function() {
		/*
		 Infotex.modalOn();
		 Infotex.loaderOn();

		 $.ajax({
		 url: '/send-message?jx=1',
		 method: 'get',
		 dataType: 'json',
		 success: function(data) {
		 Infotex.loaderOff();
		 Infotex.modalOff();

		 $('#captcha_form img').attr('src', data.captcha.captcha_name + '.gif');
		 $('#message_form input[name="_cs"]').val(data.captcha.session_id);
		 }
		 });
		 */

		$('#captcha_form').submit(function(event) {
			event.preventDefault();
			Infotex.modalOn();
			Infotex.loaderOn();

			if ($('#subject').val() == '') {
				alert("Вы забыли заполнить тему сообщения");
				Infotex.loaderOff();
				Infotex.modalOff();
				return false;
			}
			if ($('#message').val() == '') {
				alert("Вы забыли заполнить текст сообщения");
				Infotex.loaderOff();
				Infotex.modalOff();
				return false;
			}

			//$('#message_form input[name="_cn"]').val( $('#captcha_form input[name="_cn"]').val() );
			if ($('#file_form input[name="file[]"]').val()) {
				$('#file_form').submit();
			} else {
				$('#message_form').submit();
			}
			return false;
		});

		$('#file_form').ajaxForm({
			dataType: 'json',
			success: function (data) {
				if (data.error) {
					alert(data.msg);
					Infotex.loaderOff();
					Infotex.modalOff();
					return;
				}

				if (data.success) {
					if (data.files[0].filename) {
						$('#message_form input[name="d[8]"]').val( Infotex.images_dir + data.files[0].filename );
						$('#message_form').submit();
						return;
					}
				}

				alert('Произошла неизвестная ошибка');
				Infotex.loaderOff();
				Infotex.modalOff();
			}
		});

		$('#message_form').ajaxForm({
			dataType: 'json',
			success: function (data) {
				if (data.posted) {
					$('#form').html('<h2>Ваше сообщение отправлено</h2><a href="/user?mode=messages">Отправить еще одно</a>');
				} else {
					/*
					 $('#captcha_form img').attr('src', data.captcha.captcha_name + '.gif');
					 $('#message_form input[name="_cs"]').val(data.captcha.session_id);

					 if (data.captcha_error) {
					 $('#captcha_form img').before('<div class="error">Вы ввели неверный код<br></div>');
					 }
					 */
					if (data.dictionaries[6].error) {
						$('#message_form input[name="d[6]"]').before('<div class="error">' + data.dictionaries[6].error + '<br></div>');
					}
					if (data.dictionaries[7].error) {
						$('#message_form input[name="d[7]"]').before('<div class="error">' + data.dictionaries[7].error + '<br></div>');
					}
				}

				Infotex.loaderOff();
				Infotex.modalOff();
			}
		});
	},
	bindOrderImportForm: function() {
		$('#order_import_form').ajaxForm({
			dataType: 'json',
			success: function (data) {
				if (data.error) {
					alert(data.msg);
					Infotex.loaderOff();
					Infotex.modalOff();
					return;
				}

				if (data.success) {
					if (data.cart.length > 0) {
						Infotex.shop_id = $('#order_import_form input[name=shop_id]').val();

						Infotex.cookie = data.cart.join(';');
						Infotex.saveCookie();
						createCookie('CART_TOTAL_' + Infotex.shop_id, data.total, 10);
						createCookie('CART_TOTAL_AMOUNT_' + Infotex.shop_id, data.total_amount, 10);

						$('#order_import').html('<h2>Ваш заказ создан, обработано ' + data.cart.length + ' товаров</h2>' +
							(data.skipped.length > 0 ? ('<p>Список пропущенных кодов:<br>' + data.skipped.join('<br>') + '</p>') : '') +
							'<p>Вы можете <a href="/shop?mode=cart">Просмотреть свой заказ</a> или ' +
							'<a href="/user?mode=import">Попробовать импортировать еще раз</a></p>');
					} else if (data.skipped.length > 0) {
						$('#order_import').html('<h2>Не было найдено ни одного товара по указаным кодам</h2>' +
							'<p>Список пропущенных кодов:<br>' + data.skipped.join('<br>') + '</p>' +
							'<p>Вы можете <a href="/user?mode=import">Попробовать импортировать еще раз</a></p>');
					} else {
						alert('При импорте не было обработано ни одной позиции. Проверьте формат загружаемого файла.');
					}
					Infotex.loaderOff();
					Infotex.modalOff();
					return;
				}

				alert('Произошла неизвестная ошибка');
				Infotex.loaderOff();
				Infotex.modalOff();
			}
		});
		$('#order_import .submit').click(function(event) {
			event.preventDefault();
			Infotex.modalOn();
			Infotex.loaderOn();
			if (confirm('Вы уверены? Ваш текущий заказ будет перезаписан.')) {
				$('#order_import_form').submit();
			} else {
				Infotex.loaderOff();
				Infotex.modalOff();
			}
			return false;
		});
	},
	saveCookie: function(force, cb) {
		if (Infotex.cookie === false) {
			if ($.isFunction(cb)) {
				cb();
			}
			return;
		}

		var t = (new Date()).getTime();
		if (!force && (Infotex.ajax.xhr || t - 10000 < Infotex.ajax.t)) {
			window.setTimeout(function() {
				if (t > Infotex.ajax.t) {
					Infotex.saveCookie(cb);
				}
			}, 10000);
		} else {
			Infotex.ajax.xhr = $.ajax({
				url: '/shop?mode=save_cart',
				type: 'POST',
				data: {'raw': Infotex.cookie},
				dataType: 'json',
				success: function(data) {
					Infotex.ajax.xhr = false;
					Infotex.ajax.t = 0;
					if (data.error) {
						alert(data.msg);
						return;
					}

					if (data.success) {
						Infotex.ajax.t = (new Date()).getTime();
						if ($.isFunction(cb)) {
							cb();
						}
						return;
					}

					alert('Произошла неизвестная ошибка');
				}
			});
		}
	}

};



function h_u() {

	function top_menu() {
		$('.menutop ul').parent().each(function() {
			var o = $(this);
			var s = o.find('>ul');
			var l = o.parents('ul').length;
			var k = false;
			var pl = parseInt($('.menutop ul').css('paddingLeft'));
			var pr = parseInt($('.menutop ul').css('paddingRight'));
			var pb = parseInt($('.menutop ul').css('paddingBottom'));
			var pt = parseInt($('.menutop ul').css('paddingTop'));
			var bl = parseInt($('.menutop ul').css('borderLeftWidth'));
			var br = parseInt($('.menutop ul').css('borderRightWidth'));
			var bb = parseInt($('.menutop ul').css('borderBottomWidth'));
			var bt = parseInt($('.menutop ul').css('borderTopWidth'));
			o.hover(
				function() {
					o.find('>a').attr('class', 'cur');
					for (i = $('#menutop ul').length; i >= 0; i--) {
						o.parent().find('>li').not(o).find('ul').eq(i).hide();
					}
					k = true;
					var p = o.position();
					var ts, ls;
					if (l == 1) {
						ts = 5 + o.height();
						ls = 30;
					} else {
						ts = p.top - pt - bt;
						ls = p.left + o.outerWidth() + bl + 3;
					}
					s.css({
						top: ts,
						left: ls
					}).show();
				},
				function() {
					o.find('>a').attr('class', 'normal');
					k = false;
					window.setTimeout(function() {
						if (!k) {
							s.hide();
						}
					}, 500);
				}
			);
		});
	}

	top_menu();

	/* styling select .nav_pages select */

	function select_styling() {

		var s = $('.nav_pages select'); /* находим селекторы что надо оформить */
		var r = 20;
		s.each(function (i,e){
			var t  = $(e),
				width = t.outerWidth(),
				height = t.outerHeight(),
				color = t.css("color"),
				lineheight = t.css("lineHeight"),
				fontweight = t.css("fontWeight"),
				fontstyle = t.css("fontStyle"),
				fontsize = t.css("fontSize"),
				fontfamily = t.css("fontFamily"),
				margintop = t.css("marginTop"),
				marginright = t.css("marginRight"),
				marginbottom = t.css("marginBottom"),
				marginleft = t.css("marginLeft"),
				float = t.css("float"),
				verticalalign = t.css("vertical-align");
			t.wrap("<div class='styled_select'></div>"); /* оборачиваем селектор */
			t.before('<span class="bg"><span class="text"></span><span class="bg_r"></span></span>'); /* еще код для оформления */
			var wr = t.parent(),
				bg = t.prev('.bg'),
				bg_r = bg.find('.bg_r'),
				txt = bg.find('.text'),
				intervalID;
			wr.css({
				width: width,
				/*height: height,*/
				marginTop : margintop,
				marginRight : marginright,
				marginBottom : marginbottom,
				marginLeft : marginleft,
				fontWeight : fontweight,
				fontStyle : fontstyle,
				fontSize : fontsize,
				fontFamily : fontfamily,
				lineHeight : lineheight,
				color : color,
				verticalAlign: verticalalign/*,
				 float : float*/
			});
			t.css({
				position: 'relative',
				opacity: 0,
				margin : 0,
				float : "none"
			});
			set_txt(t,txt); /* выводим текст */
			t.bind('focus',function(){
				intervalID = setInterval(function(){set_txt(t,txt)},1); /* вывод текста каждую миллисекунду при получения фокуса */

			});
			t.bind('blur',function(){
				clearInterval(intervalID); /* снимаем вывод текста в каждую миллисекунду */
			});
			t.bind("focus",function(){wr.addClass("focused");});
			t.bind("blur",function(){wr.removeClass("focused");});
		});

		/*
		 *   функция для вывода текста опшна по проверке значения селектора
		 */
		function set_txt(sel,txt_box){
			var val = sel.val();
			var txt;
			var o = sel.find('option');
			o.each(function(){
				if ($(this).val()==val){
					txt = $(this).text();
					if (txt=="Выберите валюту"){
						txt = "<span class='maroon_col'>Выберите</span> валюту";
					}
					return false;
				}
			});
			if (txt!=undefined){
				txt_box.html(txt);
			}
			else {
				txt_box.html('');
			}
		}

	}

	select_styling();

	/* right menu */
	/*
	 var a = $('.menuright a');

	 function check_cur() {
	 a.each(function() {
	 if ($(this).parents('li:first').find('>ul').is(':visible')) {
	 $(this).addClass('cur');
	 }
	 else {
	 $(this).removeClass('cur');
	 }
	 });
	 }

	 a.bind('click', function(e) {
	 if ($(this).parents('li:first').find('>ul').size() > 0) {
	 e.preventDefault();
	 $(this).parents('li:first').find('>ul').toggle();
	 check_cur();
	 }
	 });

	 $('#current').find('ul:first').show();
	 $('#current').parents('ul').show();
	 check_cur();
	 */
}

$(function() {

	Infotex.onLoad();

	/*
	$(window).on('hashchange', function() {
		if (Infotex.prevent_hash) {
			Infotex.prevent_hash = false;
		} else {
			var url = window.location.hash;
			if (!~url.indexOf('#') && url.length > 1) {
				url = url.substr(1);
				Infotex.load(url);
			} else {
				Infotex.load('/');
			}
		}
	});
    */
});