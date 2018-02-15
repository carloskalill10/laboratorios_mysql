(function(){
  angular.module('app').controller('labCtrl',[
    '$scope',
    '$http',
    labController

  ])

  function labController($scope,$http){

    const url='http://localhost:3000/laboratories'
    $scope.lab ={}
    $scope.listarLab=function () {
      $http.get(url).then(function(response){
        $scope.labs =response.data
      })
    }

    $scope.create=function(){
      $http.post(url,$scope.lab).then(function(response) {
        $scope.lab ={}
        alert('Laboratório cadastrado')
      }).catch(function(resp){
        alert('falha na operção')
      })
    }

    $scope.iniciarModalDeletarL=function(lab){
      $('#modal-Deletar').modal('show')
      $scope.lab=lab
      $scope.listarLab()

    }

    $scope.fecharModalDeletarL=function(){
      $('#modal-Deletar').modal('hide');
      $scope.listarLab()
    }

    $scope.desabilitar=function () {
      let desabilitar = url+"/"+$scope.lab.id
      $http.delete(desabilitar).then(function(response){
        $scope.fecharModalDeletarL()
        alert('Laboratorio Excluido')
      }).catch(function(resp){
        $scope.fecharModalDeletarL()
        alert('Erro na exclusão')
      })
    }

    $scope.iniciarModal=function(lab){
      $('#modal-atualizar').modal('show')
      $scope.lab=lab
      $scope.listarLab()

    }

    $scope.fecharModal=function(){
      $('#modal-atualizar').modal('hide');
      $scope.listarLab()
    }

    $scope.atualizarLab=function(){
      let atualizar =url+"/"+$scope.lab.id

      $http.put(atualizar,$scope.lab).then(function(response){
        $scope.fecharModal()
        alert('Atualização Realizada')

      }).catch(function(resp){
        $scope.fecharModal()
        alert('Erro na Atualização')

      })
    }

    $scope.listarLab()
  }

})()
