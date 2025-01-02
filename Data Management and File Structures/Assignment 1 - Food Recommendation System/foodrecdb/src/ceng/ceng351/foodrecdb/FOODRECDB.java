package ceng.ceng351.foodrecdb;
import java.sql.*;
import java.util.ArrayList;

public class FOODRECDB implements IFOODRECDB{
    private static String user = "e2448843"; // TODO: Your userName
    private static String password = "NfrOiFDH234V3J-7"; //  TODO: Your password
    private static String host = "momcorp.ceng.metu.edu.tr"; // host name
    private static String database = "db2448843"; // TODO: Your database name
    private static int port = 8080; // port

    private static Connection connection = null;







    @Override
    public void initialize() {
        String url = "jdbc:mysql://" + host + ":" + port + "/" + database;

        try {
            Class.forName("com.mysql.cj.jdbc.Driver");
            connection =  DriverManager.getConnection(url, user, password);
        }
        catch (SQLException | ClassNotFoundException e) {
            e.printStackTrace();
        }

    }






    @Override
    public int createTables() {

        int numberofTablesCreated = 0;


        String queryCreateMenuItemsTable = "CREATE TABLE MenuItems (" +
                "itemID INT ," +
                "itemName VARCHAR(40) ," +
                "cuisine VARCHAR(20) ," +
                "price INT ," +
                "PRIMARY KEY (itemID))";


        String queryCreateIngredientsTable = "CREATE TABLE Ingredients(" +
                "ingredientID INT ," +
                "ingredientName VARCHAR(40) ," +
                "PRIMARY KEY (ingredientID))";



        String queryCreateIncludesTable = "CREATE TABLE Includes (" +
                "itemID INT," +
                "ingredientID INT," +
                "PRIMARY KEY (itemID, ingredientID)," +
                "FOREIGN KEY (itemID) REFERENCES MenuItems(itemID) ON DELETE CASCADE ON UPDATE CASCADE," +
                "FOREIGN KEY (ingredientID) REFERENCES Ingredients(ingredientID) ON DELETE CASCADE ON UPDATE CASCADE)";




        String queryCreateRatingsTable = "CREATE TABLE Ratings (" +
                "ratingID INT," +
                "itemID INT," +
                "rating INT," +
                "ratingDate DATE," +
                "PRIMARY KEY (ratingID)," +
                "FOREIGN KEY (itemID) REFERENCES MenuItems(itemID) ON DELETE CASCADE ON UPDATE CASCADE)";


        String queryCreateDietaryCategoriesTable = "CREATE TABLE DietaryCategories (" +
                "ingredientID INT," +
                "dietaryCategory VARCHAR(20) ," +
                "PRIMARY KEY (ingredientID, dietaryCategory)," +
                "FOREIGN KEY (ingredientID) REFERENCES Ingredients(ingredientID) ON DELETE CASCADE ON UPDATE CASCADE)";




        try {
            Statement statement = this.connection.createStatement();

            //User Table
            statement.executeUpdate(queryCreateMenuItemsTable);
            numberofTablesCreated++;

            //Song Table
            statement.executeUpdate(queryCreateIngredientsTable);
            numberofTablesCreated++;

            //Artist Table
            statement.executeUpdate(queryCreateIncludesTable);
            numberofTablesCreated++;

            //Album Table
            statement.executeUpdate(queryCreateRatingsTable);
            numberofTablesCreated++;

            //Listen Table
            statement.executeUpdate(queryCreateDietaryCategoriesTable);
            numberofTablesCreated++;

            //close
            statement.close();
        }

        catch (SQLException e) {
            e.printStackTrace();
        }

        return numberofTablesCreated;



    }

    @Override
    public int dropTables() {

        int Dropped = 0;

        String queryDropMenuItemsTable = "DROP TABLE IF EXISTS MenuItems;";

        String queryDropIngredientsTable = "DROP TABLE IF EXISTS Ingredients;";

        String queryDropIncludesTable = "DROP TABLE IF EXISTS Includes;";

        String queryDropRatingsTable = "DROP TABLE IF EXISTS Ratings;";

        String queryDropDietaryCategoriesTable = "DROP TABLE IF EXISTS DietaryCategories;";


        try {
            Statement statement = this.connection.createStatement();

            statement.executeUpdate(queryDropRatingsTable);
            Dropped++;

            statement.executeUpdate(queryDropDietaryCategoriesTable);
            Dropped++;

            statement.executeUpdate(queryDropIncludesTable);
            Dropped++;

            statement.executeUpdate(queryDropIngredientsTable);
            Dropped++;

            statement.executeUpdate(queryDropMenuItemsTable);
            Dropped++;

            //close
            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }
        return Dropped;





    }

    @Override
    public int insertMenuItems(MenuItem[] items) {
        int TotalInsertedRows = 0;

        for (int i = 0; i < items.length; i++)
        {
            try {
                MenuItem mymenu = items[i];

                PreparedStatement mystatement=this.connection.prepareStatement("INSERT INTO MenuItems VALUES(?,?,?,?)");
                mystatement.setInt(1,mymenu.getItemID());
                mystatement.setString(2,mymenu.getItemName());
                mystatement.setString(3,mymenu.getCuisine());
                mystatement.setInt(4,mymenu.getPrice());

                mystatement.executeUpdate();

                mystatement.close();
                TotalInsertedRows ++;

            }
            catch (SQLException e) {
                e.printStackTrace();
            }
        }

        return TotalInsertedRows ;

    }

    @Override
    public int insertIngredients(Ingredient[] ingredients) {
        int TotalInsertedRows = 0;

        for (int i = 0; i < ingredients.length; i++)
        {
            try {
                Ingredient myingredient = ingredients[i];

                PreparedStatement mystatement=this.connection.prepareStatement("INSERT INTO Ingredients VALUES(?,?)");
                mystatement.setInt(1,myingredient.getIngredientID());
                mystatement.setString(2,myingredient.getIngredientName());

                mystatement.executeUpdate();

                mystatement.close();
                TotalInsertedRows++;

            }
            catch (SQLException e) {
                e.printStackTrace();
            }
        }

        return TotalInsertedRows;



    }

    @Override
    public int insertIncludes(Includes[] includes) {
        int TotalInsertedRows = 0;

        for (int i = 0; i < includes.length; i++)
        {
            try {
                Includes myincludes = includes[i];

                PreparedStatement mystatement=this.connection.prepareStatement("INSERT INTO Includes VALUES(?,?)");
                mystatement.setInt(1,myincludes.getItemID());
                mystatement.setInt(2,myincludes.getIngredientID());

                mystatement.executeUpdate();

                mystatement.close();
                TotalInsertedRows++;

            }
            catch (SQLException e) {
                e.printStackTrace();
            }
        }

        return TotalInsertedRows;


    }

    @Override
    public int insertDietaryCategories(DietaryCategory[] categories) {
        int TotalInsertedRows = 0;

        for (int i = 0; i < categories.length; i++)
        {
            try {
                DietaryCategory mycategories = categories[i];

                PreparedStatement mystatement=this.connection.prepareStatement("INSERT INTO DietaryCategories VALUES(?,?)");
                mystatement.setInt(1,mycategories.getIngredientID());
                mystatement.setString(2,mycategories.getDietaryCategory());

                mystatement.executeUpdate();

                mystatement.close();
                TotalInsertedRows++;

            }
            catch (SQLException e) {
                e.printStackTrace();
            }
        }

        return TotalInsertedRows;
    }

    @Override
    public int insertRatings(Rating[] ratings) {
       int TotalInsertedRows = 0;

        for (int i = 0; i < ratings.length; i++)
        {
            try {
                Rating myratings = ratings[i];

                PreparedStatement mystatement=this.connection.prepareStatement("INSERT INTO Ratings VALUES(?,?,?,?)");
                mystatement.setInt(1,myratings.getRatingID());
                mystatement.setInt(2,myratings.getItemID());
                mystatement.setInt(3,myratings.getRating());
                mystatement.setString(4,myratings.getRatingDate());

                mystatement.executeUpdate();

                mystatement.close();
                TotalInsertedRows ++;

            }
            catch (SQLException e) {
                e.printStackTrace();
            }
        }

        return TotalInsertedRows ;


    }

    @Override
    public MenuItem[] getMenuItemsWithGivenIngredient(String name) {



        String sql = "SELECT DISTINCT M.itemID, M.itemName, M.cuisine, M.price FROM MenuItems M, Ingredients I , Includes Ing " +
                "WHERE I.ingredientName=? AND I.ingredientID=Ing.ingredientID AND Ing.itemID = M.itemID " +
                "ORDER BY M.itemID";

        try {
            PreparedStatement mystatement = this.connection.prepareStatement(sql);
            mystatement.setString(1, name);



            ResultSet result =  mystatement.executeQuery();
            ArrayList<MenuItem> results = new ArrayList<>(); //emin degilim

            while (result.next()) {
                results.add(new MenuItem(
                        result.getInt("itemID"),
                        result.getString("itemName"),
                        result.getString("cuisine"),
                        result.getInt("price")
                ));
            }

            return results.toArray(new MenuItem[0]);
        } catch (SQLException e) {
            e.printStackTrace();
        }

        return new MenuItem[0];




    }

    @Override
    public MenuItem[] getMenuItemsWithoutAnyIngredient() {
        String sql = "SELECT DISTINCT M2.itemID, M2.itemName, M2.cuisine, M2.price FROM MenuItems M2 " +
                "WHERE M2.itemID NOT IN " +
                "(SELECT M.itemID " +
                "FROM MenuItems M, Includes Inc " +
                "WHERE M.itemID = Inc.itemID) " +
                "ORDER BY M2.itemID";



        /*String sql = "SELECT DISTINCT M.itemID, M.itemName, M.cuisine, M.price FROM MenuItems M, Ingredients I , Includes Ing " +
                "WHERE Ing.ingredientID IS NULL AND Ing.itemID = M.itemID AND Ing.ingredientID=I.ingredientID " +
                "ORDER BY M.itemID";*/





        try {
            PreparedStatement mystatement = this.connection.prepareStatement(sql);

            ResultSet result =  mystatement.executeQuery();
            ArrayList<MenuItem> results = new ArrayList<>(); //emin degilim

            while (result.next()) {
                results.add(new MenuItem(
                        result.getInt("itemID"),
                        result.getString("itemName"),
                        result.getString("cuisine"),
                        result.getInt("price")
                ));
            }

            return results.toArray(new MenuItem[0]);
        } catch (SQLException e) {
            e.printStackTrace();
        }

        return new MenuItem[0];


    }

    @Override
    public Ingredient[] getNotIncludedIngredients() {

        String sql = "SELECT DISTINCT I.ingredientID, I.ingredientName FROM MenuItems M, Ingredients I , Includes Inc " +
                "WHERE  I.ingredientID NOT IN  " +
                "(SELECT  I2.ingredientID FROM Includes I2)  " +
                "ORDER BY I.ingredientID";


        try {
            PreparedStatement mystatement = this.connection.prepareStatement(sql);

            ResultSet result =  mystatement.executeQuery();
            ArrayList<Ingredient> results = new ArrayList<>(); //emin degilim

            while (result.next()) {
                results.add(new Ingredient(
                        result.getInt("ingredientID"),
                        result.getString("ingredientName")
                ));
            }

            return results.toArray(new Ingredient[0]);
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return new Ingredient[0];

    }

    @Override
    public MenuItem getMenuItemWithMostIngredients() {

        String sql = "SELECT itemID,itemName, cuisine, price FROM MenuItems " +
                "WHERE  itemID in  " +
                "(SELECT itemID FROM" +
                "(SELECT itemID, MAX(counts) AS maximum FROM " +
                "(SELECT itemID, COUNT(*) AS counts FROM Includes " +
                "GROUP BY itemID) AS T1 GROUP BY itemID ORDER BY counts DESC LIMIT 1) AS T2 ) "+
                "ORDER BY itemID";


        try {
            PreparedStatement mystatement = this.connection.prepareStatement(sql);

            ResultSet result =  mystatement.executeQuery();
            ArrayList<MenuItem> results = new ArrayList<>(); //emin degilim

            while (result.next()) {
                return(new MenuItem(
                        result.getInt("itemID"),
                        result.getString("itemName"),
                        result.getString("cuisine"),
                        result.getInt("price")
                ));
            }


        } catch (SQLException e) {
            e.printStackTrace();
        }

        return null;

    }





    @Override
    public QueryResult.MenuItemAverageRatingResult[] getMenuItemsWithAvgRatings() {

        String sql = "SELECT DISTINCT M.itemID, M.itemName, AVG(R.rating) AS myrating FROM MenuItems M " +
                "LEFT JOIN Ratings R " +
                "ON M.itemID=R.itemID " +
                "GROUP BY M.itemID, M.itemName "+
                "ORDER BY myrating DESC";

        try {
            PreparedStatement mystatement = this.connection.prepareStatement(sql);

            ResultSet result =  mystatement.executeQuery();
            ArrayList<QueryResult.MenuItemAverageRatingResult> results = new ArrayList<>(); //emin degilim

            while (result.next()) {
                results.add(new QueryResult.MenuItemAverageRatingResult(
                        result.getString("itemID"),
                        result.getString("itemName"),
                        result.getString("myrating")
                ));
            }

            return results.toArray(new QueryResult.MenuItemAverageRatingResult[0]);
        } catch (SQLException e) {
            e.printStackTrace();
        }

        return new QueryResult.MenuItemAverageRatingResult[0];


    }

    @Override
    public MenuItem[] getMenuItemsForDietaryCategory(String category) {
        String sql = "SELECT M2.itemID, M2.itemName, M2.cuisine, M2.price FROM MenuItems M2 " +
                "WHERE M2.itemID NOT IN  " +
                "(SELECT M.itemID FROM MenuItems M, Includes Inc " +
                "WHERE M.itemID=Inc.itemID AND Inc.ingredientID IN   " +
                "(SELECT DISTINCT D3.ingredientID " +
                "FROM DietaryCategories D3 " +
                "WHERE D3.dietaryCategory <> ? AND D3.ingredientID NOT IN " +
                "(SELECT D2.ingredientID " +
                "FROM DietaryCategories D2 " +
                "WHERE D2.dietaryCategory = ?) ) ) AND M2.itemID IN " +
                "(SELECT M3.itemID FROM MenuItems M3 " +
                "WHERE M3.itemID IN " +
                "(SELECT M4.itemID FROM MenuItems M4, Includes Inc4 " +
                "WHERE M4.itemID = Inc4.itemID)) " +
                "ORDER BY M2.itemID";




        try {
            PreparedStatement mystatement = this.connection.prepareStatement(sql);
            mystatement.setString(1, category);
            mystatement.setString(2, category);



            ResultSet result =  mystatement.executeQuery();
            ArrayList<MenuItem> results = new ArrayList<>(); //emin degilim

            while (result.next()) {
                results.add(new MenuItem(
                        result.getInt("itemID"),
                        result.getString("itemName"),
                        result.getString("cuisine"),
                        result.getInt("price")
                ));
            }

            return results.toArray(new MenuItem[0]);
        } catch (SQLException e) {
            e.printStackTrace();
        }




        return new MenuItem[0];
    }

    @Override
    public Ingredient getMostUsedIngredient() {




        String sql = "SELECT ingredientID,ingredientName FROM Ingredients " +
                "WHERE  ingredientID in  " +
                "(SELECT ingredientID FROM" +
                "(SELECT ingredientID, MAX(counts) AS maximum FROM " +
                "(SELECT ingredientID, COUNT(*) AS counts FROM Includes " +
                "GROUP BY ingredientID) AS T1 GROUP BY ingredientID ORDER BY counts DESC LIMIT 1) AS T2 ) ";


        try {
            PreparedStatement mystatement = this.connection.prepareStatement(sql);

            ResultSet result =  mystatement.executeQuery();
            ArrayList<Ingredient> results = new ArrayList<>(); //emin degilim

            while (result.next()) {
                return (new Ingredient(
                        result.getInt("ingredientID"),
                        result.getString("ingredientName")
                ));
            }


        } catch (SQLException e) {
            e.printStackTrace();
        }

        return null;

    }




    @Override
    public QueryResult.CuisineWithAverageResult[] getCuisinesWithAvgRating() {


        String sql = "SELECT DISTINCT M.cuisine,  AVG(R.rating) AS myrating FROM MenuItems M " +
                "LEFT JOIN Ratings R " +
                "ON M.itemID=R.itemID " +
                "GROUP BY M.cuisine " +
                "ORDER BY myrating DESC";



        try {
            PreparedStatement mystatement = this.connection.prepareStatement(sql);

            ResultSet result =  mystatement.executeQuery();
            ArrayList<QueryResult.CuisineWithAverageResult> results = new ArrayList<>(); //emin degilim

            while (result.next()) {
                results.add(new QueryResult.CuisineWithAverageResult(
                        result.getString("cuisine"),
                        result.getString("myrating")
                ));
            }

            return results.toArray(new QueryResult.CuisineWithAverageResult[0]);
        } catch (SQLException e) {
            e.printStackTrace();
        }



        return new QueryResult.CuisineWithAverageResult[0];
    }

    @Override
    public QueryResult.CuisineWithAverageResult[] getCuisinesWithAvgIngredientCount() {

        /* String sql = "SELECT DISTINCT X.cuisine,  AVG(X.myinc) AS average FROM " +
                "(SELECT M.cuisine, " +
                "(SELECT COUNT(*) FROM Includes I " +
                "WHERE M.itemID = I.itemID) AS myinc FROM MenuItems M) X " +
                 "GROUP BY X.cuisine " ; */

        String sql = "SELECT DISTINCT X.cuisine,  AVG(X.numIngredients) AS average FROM " +
                "(SELECT M.cuisine, COUNT(I.itemID) AS numIngredients FROM MenuItems M " +
                "LEFT JOIN Includes I " +
                "ON M.itemID = I.itemID " +
                "GROUP BY M.itemID) X " +
                "GROUP BY X.cuisine " +
                "ORDER BY average DESC" ;










        try {
            PreparedStatement mystatement = this.connection.prepareStatement(sql);

            ResultSet result =  mystatement.executeQuery();
            ArrayList<QueryResult.CuisineWithAverageResult> results = new ArrayList<>(); //emin degilim

            while (result.next()) {
                results.add(new QueryResult.CuisineWithAverageResult(
                        result.getString("cuisine"),
                        result.getString("average")
                ));
            }

            return results.toArray(new QueryResult.CuisineWithAverageResult[0]);
        } catch (SQLException e) {
            e.printStackTrace();
        }







        return new QueryResult.CuisineWithAverageResult[0];
    }

    @Override
    public int increasePrice(String ingredientName, String increaseAmount) {
        int noRows = 0;
        int number = Integer.parseInt(increaseAmount);

        try {

            PreparedStatement mystatement=this.connection.prepareStatement("UPDATE MenuItems M\n" +
                    "SET    M.price = M.price +?\n" +
                    "WHERE   M.itemID= \n" +
                    "(SELECT M.itemID \n" +
                    "FROM  Ingredients I, Includes Inc \n" +
                    "WHERE Inc.ingredientID=I.ingredientID AND I.ingredientName=? AND Inc.itemID=M.itemID)");
            mystatement.setInt(1,number);
            mystatement.setString(2,ingredientName);

            noRows=mystatement.executeUpdate();

            //close
            mystatement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        return noRows;
    }






    @Override
    public Rating[] deleteOlderRatings(String date) {
        String sql2 = "SELECT R.ratingID, R.itemID, R.rating, R.ratingDate FROM Ratings R " +
                "WHERE R.ratingDate < ?  " +
                "ORDER BY R.ratingID";




        try {
            PreparedStatement mystatement = this.connection.prepareStatement(sql2);
            mystatement.setString(1, date);




            ResultSet result =  mystatement.executeQuery();
            ArrayList<Rating> results = new ArrayList<>(); //emin degilim

            while (result.next()) {
                results.add(new Rating(
                        result.getInt("ratingID"),
                        result.getInt("itemID"),
                        result.getInt("rating"),
                        result.getString("ratingDate")
                ));
            }

            return results.toArray(new Rating[0]);
        } catch (SQLException e) {
            e.printStackTrace();
        }











        String sql = "DELETE FROM Ratings R WHERE R.ratingDate < ?";

        try {
            PreparedStatement mystatement = this.connection.prepareStatement(sql);

            mystatement.setString(1, date);

            mystatement.executeUpdate();

            try {
                mystatement = this.connection.prepareStatement("SELECT COUNT(*) FROM Ratings");

                ResultSet resultSet = mystatement.executeQuery();


            } catch (SQLException e) {
                e.printStackTrace();
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }









        return new Rating[0];
    }
}
