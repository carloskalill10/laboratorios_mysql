module.exports = app => {
    const controllerReserve = app.controller.reserve;

    app.route("/reserve")
        .post(controllerReserve.createReserve)

    app.route("/reserve/:id")
        .delete(controllerReserve.delete);
    app.route("/reserve/:date/:lab").get(controllerReserve.findAcessForDateAndLab);
};
