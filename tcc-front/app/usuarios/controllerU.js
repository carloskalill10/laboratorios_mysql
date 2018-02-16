(function(){
  angular.module('app').controller('userCtrl',[
    '$scope',
    '$http',
    userController
  ])

  function userController($scope,$http){
    const url=['http://localhost:3000/users','http://localhost:3000/insert_aux']
    $scope.userTipo=false
    $scope.user={}
    $scope.user.tipo=false


    $scope.listaruser=function () {
     
      $http.get(url[0]).then(function(response){
        let users =response.data
        $scope.users=[]
        users.forEach(function(data){
          if (data.funcao==0){
            data.funcao="PROFESSOR"
          }else if(data.funcao==1){
            data.funcao="ALUNO"
          }else if(data.funcao==2){
            data.funcao="ADMINISTRATIVO"
          }else if(data.funcao==3){
            data.funcao="TECNICO"
          }else if(data.funcao==4){
            data.funcao="OUTRA"
          }

          if (data.tipo==1){
            data.tipo='ADMINISTRADOR'
            $scope.users.push(data)
          }else{
            data.tipo='COMUM'
            $scope.users.push(data)
          }
        })
        
      })
    }

    $scope.create=function(){
      if($scope.user.tipo==false){
        $scope.user.tipo=0
      }
      $http.post(url[0],$scope.user).then(function(response) {
        $scope.user ={}
        alert('Usuário cadastrado')
      }).catch(function(resp){
        alert('falha na operção')
      })
    }

    $scope.iniciarModalDeletarU=function(user){
      $('#modal-DeletarU').modal('show')
      $scope.user=user
      var str1="ADMINISTRADOR"
      if(str1==$scope.user.tipo){
          $scope.userTipo=true
      }else{
          $scope.userTipo=false
      }

      if ($scope.user.funcao==0){
        $scope.user.funcao=PROFESSOR
      }else if($scope.user.funcao==1){
        $scope.user.funcao=ALUNO
      }else if($scope.user.funcao==2){
        $scope.user.funcao=ADMINISTRATIVO
      }else if($scope.user.funcao==3){
        $scope.user.funcao=TECNICO
      }else if($scope.user.funcao==4){
        $scope.user.funcao=OUTRA
      }

    }

    $scope.fecharModalDeletarU=function(){
      $('#modal-DeletarU').modal('hide');
      $scope.listaruser()
    }

    $scope.desabilitar=function () {
       let desabilitar = url[0]+"/"+$scope.user.id
       $http.delete(desabilitar).then(function(response){
         $scope.fecharModalDeletarU()
         alert('Usuário Excluído')
       }).catch(function(resp){
          $scope.fecharModalDeletarU()
         alert('Erro na exclusão')
       })
     }

     $scope.habilitarForm=function(){
      $http.get(url[1]).then(function(response){
        const retorno = response.data;
        if(retorno.length!=0){
          $scope.user.tag=retorno[0].tag;
        }
      });
    }

     $scope.iniciarModalU=function(user){
       $('#modal-atualizarU').modal('show')
       $scope.user=user
       var str1="ADMINISTRADOR"
       if(str1==$scope.user.tipo){
          $scope.user.tipo=true
       }else{
          $scope.user.tipo=false
       }

      if ($scope.user.funcao==0){
        $scope.user.funcao=PROFESSOR
      }else if($scope.user.funcao==1){
        $scope.user.funcao=ALUNO
      }else if($scope.user.funcao==2){
        $scope.user.funcao=ADMINISTRATIVO
      }else if($scope.user.funcao==3){
        $scope.user.funcao=TECNICO
      }else if($scope.user.funcao==4){
        $scope.user.funcao=OUTRA
      }
     }

     $scope.fecharModalU=function(){
       $('#modal-atualizarU').modal('hide');
       $scope.listaruser()
     }

     $scope.atualizarU=function(){
       let atualizar = url[0]+"/"+$scope.user.id
       if($scope.user.tipo==true){
          $scope.user.tipo=1
       }else{
          $scope.user.tipo=0
       }

       $http.put(atualizar,$scope.user).then(function(response){
         $scope.fecharModalU()
         alert('Atualização Realizada')

       }).catch(function(resp){
         $scope.fecharModalU()
         alert('Erro na Atualização')

       })
     }

    $scope.listaruser()
    $scope.habilitarForm()
  }

})()
