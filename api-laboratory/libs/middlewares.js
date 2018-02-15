import bodyParser from "body-parser";

module.exports = app => {
    app.set("port", 3000);
    app.set("json spaces", 4);
    app.use(bodyParser.json());
    app.use(function (req, res, next) {
        res.header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS, PUT, PATCH, DELETE');
        res.header('access-Control-Allow-Origin', '*');
        res.header("Access-Control-Allow-Headers", "Content-Type, Access-Control-Allow-Headers, Authorization, X-Requested-With");
        next();
    });
    //app.use(app.auth.initialize());

    app.use((req, res, next) => {
        delete req.body.id;
        next();
    });
};
