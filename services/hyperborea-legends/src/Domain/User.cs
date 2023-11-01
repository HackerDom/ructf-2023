using System.Text.Json.Serialization;

namespace Domain;

public class User
{
    private byte[][] directAncestors { get; set; }
    public int Id { get; set; }
    public string Username { get; set; }
    public string Password { get; set; }
    public SpeciesType Species { get; set; }

    public HashSet<Guid> DirectAncestors
    {
        get => directAncestors.Select(x => new Guid(x)).ToHashSet();
        set => directAncestors = value.Select(x => x.ToByteArray()).ToArray();
    }

    public int AncestorsCount { get; set; }
}

[JsonConverter(typeof(JsonStringEnumConverter))]
public enum SpeciesType
{
    GreatRuss,
    FilthyLizard
}