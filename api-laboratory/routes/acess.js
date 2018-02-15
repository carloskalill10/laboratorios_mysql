module.exports = app => {
    const controllerAcess= app.controller.acess;

    app.route("/acess").post(controllerAcess.verifyAndCreateAcess);
    app.route("/acess/:date/:lab").get(controllerAcess.findAcessForDateAndLab);

};
