(function () {
    angular.module('app').controller('reservaCtrl', [
        '$scope',
        '$http',
        reservaController
    ])
    function reservaController($scope, $http) {
        const urls = ['http://localhost:3000/users', 'http://localhost:3000/laboratories'
            , 'http://localhost:3000/reserve'];
        $scope.users = [];
        $scope.labs = [];
        
        $scope.listarUsers = function () {
            $http.get(urls[0]).then(function (response) {
                $scope.users = response.data;
            }).catch(function (resp) {
                console.log(resp);
            });
        };
        
        $scope.listarLabs = function () {
            $http.get(urls[1]).then(function (response) {
                $scope.labs = response.data;
            }).catch(function (resp) {
                console.log(resp);
            });
        };
        
        $scope.cadastrarReserva = function () {
            let reserva = $scope.reserva;
            reserva.dt_entrada = converterData(reserva.data, reserva.hora_entrada);
            reserva.dt_saida = converterData(reserva.data, reserva.hora_saida);
            $http.post(urls[2], reserva).then(function (response) {
                $scope.reserva={};
                alert('Reserva realizada!!');
            }).catch(function (resp) {
                console.log(resp)
            });
            console.log(reserva)
        };

        $scope.listarReserva = function () {
            let data= converterOnlyData($scope.reserva.data)
            $http.get(urls[2]+"/"+data+"/"+$scope.reserva.laboratory_id).then(function (response) {
                $scope.reservas = response.data;
                
            }).catch(function (resp) {
                console.log(resp);
            });
        };


        $scope.listarUsers();
        $scope.listarLabs();
    }
    function converterData(data, hora) {
        let date = new Date(data);
        let day = date.getDate() < 10 ? "0" + date.getDate() : date.getDate();
        let month = (date.getMonth() + 1) < 10 ? "0" + (date.getMonth() + 1) : (date.getMonth() + 1);
        let hours = hora.getHours() < 10 ? "0" + hora.getHours() : hora.getHours();
        let minutes = hora.getMinutes() < 10 ? "0" + hora.getMinutes() : hora.getMinutes();
        let dataString = date.getFullYear() + "-" + month + "-" + day + " " + hours + ":" + minutes + ":00";
        return dataString;
    }

    function converterOnlyData(data) {
        let date = new Date(data);
        let day = date.getDate() < 10 ? "0" + date.getDate() : date.getDate();
        let month = (date.getMonth() + 1) < 10 ? "0" + (date.getMonth() + 1) : (date.getMonth() + 1);
        let dataString = date.getFullYear() + "-" + month + "-" + day;
        return dataString;
    }
})()