use work.channel.all;

entity newMergexample is
end newMergexample;

architecture new_structure of newMergexample is
  component winery
    port(wine_to_old_shop, wine_to_new_shop : inout channel);
  end component;

  component newMerge
    port(X, Y, Z : inout channel);
  end component;

  component patron
    port(wine_buying : inout channel);
  end component;

  signal X, Y, Z : channel := init_channel;
begin
  XYenv : winery port map(wine_to_old_shop => X, wine_to_new_shop => Y);
  theNEWMERGE : newMerge port map(X => X, Y => Y, Z => z);
  Zenv : patron port map(wine_buying => Z);
end new_structure;
