(function() {
    if (!window.console) {
	window.console = {};
    }
          // union of Chrome, FF, IE, and Safari console methods
            var m = [
            "log", "info", "warn", "error", "debug", "trace", "dir", "group",
            "groupCollapsed", "groupEnd", "time", "timeEnd", "profile", "profileEnd",
            "dirxml", "assert", "count", "markTimeline", "timeStamp", "clear"
     ];
    // define undefined methods as noops to prevent errors
    for (var i = 0; i < m.length; i++) {
	if (!window.console[m[i]]) {
	window.console[m[i]] = function() {};
	}    
    } 
})();

/* @namespace*/
var flyBasket = {
    init: function() {
        var bsBox = $('#poisk'),
            bsPosition = bsBox.position(),
            bsOffset = bsBox.offset(),
            w = $(window);

        var pBox = $('#preview'),
            pPosition = pBox.position(),
            pOffset = pBox.offset();

        var cssScrollBasket = 'bs-scrollBasket',
            cssScrollPreview = 'p-scroll';


        //if isset bsBox
        if (bsBox.length) {

            function flyBasketResize() {
                var scrollBasket = false;

                if (bsBox.hasClass(cssScrollBasket)) {
                    scrollBasket = true;
                    bsBox.removeClass(cssScrollBasket).css('left', 'auto').offset().left;
                    pBox.removeClass(cssScrollPreview).css('left', 'auto').offset().left;
                }

                bsOffset = bsBox.offset();
                bsPosition = bsBox.position();

                pPosition = pBox.position();
                pOffset = pBox.offset();

                if (scrollBasket)
                    bsBox.addClass(cssScrollBasket);
                pBox.addClass(cssScrollPreview);
                w.trigger('scroll');
            };

            //events
            window.onresize = flyBasketResize;

            w.scroll(function () {
                if (w.scrollTop() >= 200) {
                    bsBox.addClass(cssScrollBasket);
                    bsBox.css('left', bsOffset.left-13);

                    pBox.addClass(cssScrollPreview);
                    pBox.css('left', pOffset.left-13);
                }
                else {
                    bsBox.removeClass(cssScrollBasket);
                    bsBox.css('left', 'auto');

                    pBox.removeClass(cssScrollPreview);
                    pBox.css('left', 'auto');
                }
            });
        }
    }
};

var flyMenuObj = {
	 init: function() {
        var menuBlock = $('#main_content').find('.navbar-green'),
            w = $(window);

        var cssScrollMenu = 'bs-scrollMenu';

        if (menuBlock.length) {



            function menuBasketResize() {
                var scrollBasket = false;

                if (menuBlock.hasClass(cssScrollMenu)) {
                    scrollBasket = true;
                    menuBlock.removeClass(cssScrollMenu);
                }

                if (scrollBasket) {
                    menuBlock.addClass(cssScrollMenu);
                    w.trigger('scroll');
                }

            };

            //events
            window.onresize = menuBasketResize;

            w.scroll(function () {
                if (w.scrollTop() >= 100) {
                    menuBlock.addClass(cssScrollMenu);
                }
                else {
                    menuBlock.removeClass(cssScrollMenu);
                }
            });
        }
    }
}



$(function() {
	flyBasket.init();
	flyMenuObj.init();
})
