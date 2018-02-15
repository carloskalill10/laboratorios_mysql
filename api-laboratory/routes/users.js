module.exports = app => {
    const controllerUser = app.controller.user;

    app.route("/users")
        .post(controllerUser.createUser)
        .get( controllerUser.findAllUser);

    app.route("/users/:id")
        .put(controllerUser.update)
        .delete(controllerUser.disable);

};
