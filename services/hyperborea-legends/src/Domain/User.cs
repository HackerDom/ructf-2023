using System.Text.Json.Serialization;

namespace Domain;

public class User
{
    // public User(int id, string username, string password, SpeciesType species, List<byte[]> directAncestors)
    // {
    //     Id = id;
    //     Username = username;
    //     Password = password;
    //     Species = species;
    //     DirectAncestors = directAncestors.Select(x => new Guid(x)).ToList();
    // }
    private byte[][] directAncestors { get; set; }
    public int Id { get; set; }
    public required string Username { get; set; }
    public required string Password { get; set; }
    public required SpeciesType Species { get; set; }

    public required HashSet<Guid> DirectAncestors
    {
        get => directAncestors.Select(x => new Guid(x)).ToHashSet();
        set => directAncestors = value.Select(x => x.ToByteArray()).ToArray();
    }
}

[JsonConverter(typeof(JsonStringEnumConverter))]
public enum SpeciesType
{
    GreatRuss,
    FilthyLizard
}