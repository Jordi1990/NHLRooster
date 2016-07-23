package jordi.nhlrooster.DataObjects;

/**
 * Created by jordi on 21-3-15.
 */
public class SearchResult{
    public String name;
    public String department;
    public int type;
    public int id;

    public SearchResult(String name, String department, int type, int id){
        this.name = name;
        this.department = department;
        this.type = type;
        this.id = id;
    }
}