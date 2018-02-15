(function(){
  angular.module('app').factory('tabs',[TabsFactory])

  function TabsFactory(){

    function show(obj,{
      tabCreate=false,
    }){
      obj.tabCreate=tabCreate
    }
    return {show}
  }
})()
