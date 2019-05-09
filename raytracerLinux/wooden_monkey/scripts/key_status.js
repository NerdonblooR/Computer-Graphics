$(function(){
	window.keydown = {};
	function keyName(e){
		return jQuery.hotkeys.specialKeys[e.which]||String.fromCharCode(e.which).toLowerCase();
	}
	$(document).bind("keydown",function(e){
		keydown[keyName(e)]=true;
	});
	$(document).bind("keyup",function(e){
		keydown[keyName(e)]=false;
	});
});