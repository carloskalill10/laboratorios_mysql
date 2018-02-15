(function(){
  angular.module('app').controller('acessoCtrl',[
    '$scope',
    '$http',
    acessoController

  ])

  function acessoController($scope,$http){

    const urls = ['http://localhost:3000/laboratories'
    , 'http://localhost:3000/acess'];
  
    $scope.labs = [];

    $scope.listarLabs = function () {
      $http.get(urls[0]).then(function (response) {
          $scope.labs = response.data;
      }).catch(function (resp) {
          console.log(resp);
      });
    };
    
    $scope.listarAcessos = function () {
      let data= converterOnlyData($scope.acesso.data)
      $http.get(urls[1]+"/"+data+"/"+$scope.acesso.laboratory_id).then(function (response) {
          $scope.acessos = response.data;
      }).catch(function (resp) {
          console.log(resp);
      });
  };

    $scope.listarLabs()
  }

  function converterOnlyData(data) {
    let date = new Date(data);
    let day = date.getDate() < 10 ? "0" + date.getDate() : date.getDate();
    let month = (date.getMonth() + 1) < 10 ? "0" + (date.getMonth() + 1) : (date.getMonth() + 1);
    let dataString = date.getFullYear() + "-" + month + "-" + day;
    return dataString;

  }

})()
