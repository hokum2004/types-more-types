val scala3Version = "3.2.2"

val CatsVersion = "2.9.0"

lazy val root = project
  .in(file("."))
  .settings(
    name := "example",
    version := "0.1.0-SNAPSHOT",
    scalaVersion := scala3Version,
    libraryDependencies ++= Seq(
      "org.typelevel" %% "cats-core" % CatsVersion,
    ),
    scalacOptions ++= Seq(
      "-feature", // then put the next option on a new line for easy editing
      // "-language:implicitConversions",
      "-language:existentials",
      "-unchecked",
      "-Werror",
      // "-java-output-version", "18",
      "-new-syntax",
      "-indent",
      // "-rewrite",
      "-explain",
    ),
    fork := true,
  )
