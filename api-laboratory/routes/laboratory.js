module.exports = app => {
    const controllerLaboratory = app.controller.laboratory;

    app.route("/laboratories")
        .post(controllerLaboratory.createLaboratory)
        .get( controllerLaboratory.findAllLaboratory);

    app.route("/laboratories/:id")
        .put(controllerLaboratory.update)
        .delete(controllerLaboratory.disable);
 
};
