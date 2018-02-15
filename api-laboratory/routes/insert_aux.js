module.exports = app => {
    const controllerInsert_aux = app.controller.insert_aux;

    app.route("/insert_aux")
        .post(controllerInsert_aux.createInsert_aux)
        .get( controllerInsert_aux.findIntevalOneMinuteLater);


};
