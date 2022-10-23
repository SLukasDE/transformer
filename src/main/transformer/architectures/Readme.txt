Eine Build-Definition wird u.a. durch eine Aufzählung von Specifiern (Strings) angegeben.
Bei einem Specifier kann es sich dabei entweder um eine Architektur oder ein Profil handeln.

Architekturen und Profile werden in einer separaten Konfiguration beschrieben.
Darin wird für jede Architektur beschrieben, wie sich ein bestimmtes Profil auf den Kompilierungsprozess auswirkt.

  So beschreibt z.B. eine Architektur "gcc-8.4.0", dass das Verwenden eines Profils "c++11" dazu führt,
  dass der C++ Compiler zusätzlich mit dem Argument "-std=c++11" aufgerufen wird.

In der Konfiguration wird auch beschreiben, wie sich das Vorkommen mehrerer Architekturen in Kombination zusammen auswirkt.

  So beschreibt z.B. eine Architektur "amd64 linux gcc-8.4.0", dass das Vorkommen dieser Kombination zur Verwendung
  eines ganz speziellen Treibers (Compilers) führt. Wird darüber hinaus während des Builds auch das Profil "c++11"
  verwendet, so würde der hier definierte C++ Compiler mit dem zusätzlichen Argument aufgerufen werden, wie es
  für das Profil "c++11" in der Architektur "gcc-8.4.0" beschrieben ist.
  
  HINWEIS: Hätte es auch in der Definition der Architektur "amd64 linux gcc-8.4.0" eine
           Definition für das Profil "c++11" gegeben, wäre diese Defintion verwendet worden.
           Die genaue Hierarchie und Auflösung wird später erläuter.

Mögliche Profile sind somit indirekt durch die Definition der Architekturen definiert.


Bis jetzt ließe sich somit ein Build für eine exakt spezifierirter Architektur und deren Profilen festlegen. 
Es soll aber möglich sein einen Build für eine Vielzahl an Architekturen und ggf. auch Profilen zu definieren
ohne sie alle separat aufzulisten. Vielmehr sollen nur die Leitplanken definiert werden, z.B. dass das zu übersetzende
Programm einen C++11 Kompiler benötigt und damit auf jedem System kompilierbar ist.
Oder vielleicht erforder der Code nicht einen spezifischen C++ Standard, aber als Betriebssystem Linux.


1. Normalize
2. es wird jeder weitere Specifier hinzugefügt, der
   a) nicht optional ist und
   b) keinen default-Neighber (in Bezug auf Branches) hat und
   c) sich nicht durch sich selbst oder einer seiner Parents in durch einen Branch der bisher angegebenen Specifier ausschließt
      *) die Prüfung des Ausschlusses über die bisher angegebenen Specifier muss ebenfalls über die Prüfung der bisher angegebenen Specifier erfolgen,
         sowie dessen möglichen Kinder (auch hier wieder Defaults und Optionals berücksichtigen).  

1. addProfil: optA
2. addProfil: optimB
3. addProfil: optimC
4. Branch:    optA optimB optimC
5. addArch:   optim

1. "optA"   -> P: "optA"
   "opt"    -> P: "optA"
   "op"     -> P: "optA"
   "o"      -> P: "optA"
   ""       -> P: "optA"

2. "optimB" -> P: "optimB"
   "optim"  -> P: "optimB"
   "opti"   -> P: "optimB"
   "optA"   -> P: "optA"
   "opt"    -> U: "opt"
   "op"     -> U: "opt"
   "o"      -> U: "opt"
   ""       -> U: "opt"

3. "optimC" -> P: "optimC"
   "optimB" -> P: "optimB"
   "optim"  -> U: "optim"
   "opti"   -> U: "optim"
   "optA"   -> P: "optA"
   "opt"    -> U: "opt"
   "op"     -> U: "opt"
   "o"      -> U: "opt"
   ""       -> U: "opt"

5. "optimC" -> P: "optimC"
   "optimB" -> P: "optimB"
   "optim"  -> A: "optim"
   "opti"   -> A: "optim"
   "optA"   -> P: "optA"
   "opt"    -> U: "opt"
   "op"     -> U: "opt"
   "o"      -> U: "opt"
   ""       -> U: "opt"










class transformer::architectures::Architectures {
...
	/* This is a helper container used only by method "insertId(const std::string& id)".
	 *
	 * Let's assume you add first "gcc-4.8.3". In other words you call insertId("gcc-4.8.3");
	 * Later you add "gcc".
	 *
	 * Now you have 2 public specifiers "gcc" and "gcc-4.8.3" but "gcc-4.8.3" should inherit
	 * available architectures form "gcc".
	 * ...
	 *
	 * ACHTUNG: Soll das wirklich so sein?
	 *          Braucht man "specifiersBySubId" ?
	 *
	 * Angenommen in "gcc" gabe es eine Architektur mit den ids "gcc noexcept"
	 * Dann sollten die Settings für "noexcept" natürlich auch in "gcc-4.8.3" verfügbar sein.
	 * Aber dafür könnte "gcc-4.8.3" auch selbst nachschlagen, indem es nach Setting für
	 * "ggc-4.8.", "ggc-4.8", "ggc-4.", ..., "ggc-", "gcc" sucht.
	 * STATUS QUO müsste das "gcc" über seinen Parent eh machen.
	 * Also wenn als Build-Architekur "gcc-4.8.3" angegeben wird, kommt momentan "gcc" gar nicht vor.
	 * STIMMT AUCH WIEDER nicht. Es kommt "gcc" vor, wie auch "clang".
	 * Aber dann wird später "clang" verworfen, weil "gcc" am Ende der Kette fehlt.
	 *
	 * Nein, es ist vermutlich wichtiger dafür zu sorgen, dass wenn als Build-Architektur "gcc"
	 * angegeben wird auch "gcc-4.8.3" vorkommt. Würde aber auch passieren, weil sowohl "gcc",
	 * als auch "gcc-4.8.3" Root-Specifier wären, die sich nicht gegenseitig ausschließen.
	 *
	 * Bleibt noch, dass dafür gesorgt werde muss, dass "gcc-4.8.3" auch garaniert die Settings
	 * von "gcc" für gleiche Profil überschreibt. Wenn also "gcc" und "gcc-4.8.3" eine Einstellung
	 * für das Profil "noexcept" haben, dann muss die Einstellung von "gcc-4.8.3" die von "gcc"
	 * überschreiben. Das würde aber wieder später gehen, da "gcc" ein Teilstring von "gcc-4.8.3" ist.
	 *
	 * Gründe, weshalb doch eine implizite Baumstruktur erforderlich ist, also "gcc" ein Parent ist
	 * von verschiedenen "gcc-x.y.z" und es nicht reicht alles über Branches zu regeln:
	 * - wenn "gcc-4.8.3" bei der Branch-Angabe als "default" markiert wird, dann muss klar sein,
	 *   dass "gcc-8.3" nicht mehr als Default angegeben werden kann
	 *   -> PROBLEM: was ist wenn es auch ein "gcc-4" gibt? Dann findet der Branch auf niedrigerer Ebene statt.
	 *      -> Ja, dann könnte "gcc-8.3" auch als Default angegeben werden. Das ist auch so gewollt.
	 *         Gibt man bei der Build-Architektur nur "gcc" an, würde "gcc-8.3" ausgewählt werden.
	 *         Aber gibt man bei der Build-Architektur "gcc-4" an, so wird "gcc-4.8.3" ausgewählt.
	 *         Möchte man, dass auch bei "gcc" der "gcc-4.8.3" ausgewählt wird, so ist ein
	 *         Branch "gcc-8" und "gcc-4" mit Default="gcc-4" erforderlich.
	 *   -> lässt sich das nicht auch wieder später lösen?
	 *
	 * DER WICHTIGSTE (und vielleicht einzige Grund) für den Helper-Container liegt darin
	 * zu erkennen, dass es eine geimeinsame Sub-ID gibt (bzw. angelgt werden muss).
	 * Bsp.: zu erst wird "gcc-4.8.3" angelegt, danach "gcc-8.2.0".
	 *
	 * Woher weiß nun insertId("gcc-8.2.0"), dass es bei der SubID "gcc-" eine Schnittmenge gibt?
	 * Immer weiter Verkürzen "gcc-8.2.", "gcc-8.2", "gcc-8." ... "gcc", "gc", ... hilft für sich genommen
	 * noch nicht, wenn man nicht prüfen kann, ob z.B. unter "gcc" weitere Einträge (Kinder) existieren.
	 */
	 
	std::map<std::string, std::reference_wrapper<Specifier>> specifiersBySubId;
...
};
 