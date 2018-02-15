module.exports = (sequelize, DataType) => {
    const Insert_aux = sequelize.define("Insert_aux", {
        id: {
            type: DataType.INTEGER,
            primaryKey: true,
            autoIncrement: true
        },
        tag: {
            type: DataType.STRING,
            allowNull: false,
            validate: {
                notEmpty: true
            }
        }
    }, {
        classMethods: {
            associate: (models) => {
            }
        }
    });
    return Insert_aux;
};
